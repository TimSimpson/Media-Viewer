#include "browser.hpp"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <SDL_ttf.h>

namespace fs = std::experimental::filesystem;

namespace st4 { namespace media_viewer {

FileBrowser::FileBrowser(
	lp3::sdl::Window & window,
    lp3::core::MediaManager & _media,
	lp3::sdl::Renderer & _renderer,
	std::string _top_directory
)
:   media(_media),
	renderer(_renderer),
	top_directory(fs::absolute(fs::path(_top_directory))),
	texture(),
	font_file(),
	font(nullptr),
	rect(boost::none),
	index(-1),
	old_index(-1),
	max(0),
	current_directory(),
	current_folders(),
	current_files(),
    file_is_open(false),
	media_player(window)
{
	media_player.hide_bar();
	media_player.hide();

    this->font_file = media.load("fonts/Inconsolata-Regular.ttf");
	// TTF_OpenFontRW surprisingly- and almost without documentation - OWNS
	// the freakin' RWops object you pass to it so it can lazily load the font.
	// This cost me a day of my life, and is the reason the font_file has to
	// be part of this class.
	// Here is someone else on the internet who had the same issue:
	// https://github.com/andelf/rust-sdl2_ttf/issues/43
	this->font = TTF_OpenFontRW(this->font_file, 0, 56);
    if (nullptr == font) {
        LP3_LOG_ERROR(SDL_GetError());
        LP3_THROW2(lp3::core::Exception, "Error loading font!");
    }

	this->set_directory(top_directory);
	update_text();
}

FileBrowser::~FileBrowser() {
	if (texture) {
		SDL_DestroyTexture(texture);
	}
	if (font) {
		TTF_CloseFont(font);
	}
}

void FileBrowser::handle_events(const SDL_WindowEvent & window_event) {
	media_player.handle_events(window_event);
}

void FileBrowser::render() {
	if (!file_is_open && old_index != index) {
		LP3_LOG_INFO("Time to update text.")
			update_text();
		old_index = index;
	}
	SDL_RenderCopy(renderer, texture, nullptr, rect.get_ptr());
}

void FileBrowser::set_directory(const fs::path & path) {
	current_directory = path;
	current_folders.clear();
	current_files.clear();
	index = 0;
	old_index = -1;

	auto root = fs::absolute(fs::path(path));
	for (auto itr = fs::directory_iterator(root);
		itr != fs::directory_iterator{}; ++itr) {
		if (is_regular_file(*itr)) {
			current_files.push_back(itr->path().filename());
		}
		else {
			current_folders.push_back(itr->path().filename());
		}
	}
	this->max = current_folders.size() + current_files.size();
}

void FileBrowser::set_file() {
	file_is_open = true;
	const auto file = this->current_directory 
		/ this->current_files[this->index - this->current_folders.size()];
	
	std::string ext = file.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	
	SDL_Texture * new_texture = nullptr;
	if (ext == ".jpg" || ext == ".png" || ext == ".bmp") {		
		media_player.hide();
		media_player.stop();
		auto s = file.string();
		lp3::sdl::Surface image = IMG_Load(s.c_str());
		this->rect = boost::none;
		new_texture = SDL_CreateTextureFromSurface(renderer, image);
		
		
		this->rect = SDL_Rect{};
		double w = static_cast<SDL_Surface *>(image)->w;
		double h = static_cast<SDL_Surface *>(image)->h;

		if (w / h >= (1920 / 1080)) {
			// stretch to max horizontal, scale vertical
			this->rect->w = 1920;
			this->rect->h = (1920 * h) / w;
		} else {
			// stretch to max vertical, downsize horizontal
			this->rect->h = 1080;
			this->rect->w = (w * 1080) / h;
		}

		this->rect->x = 0;
		this->rect->y = 0;		
	} else if (ext == ".mp4" || ext == ".mov") {
		auto s = file.string();
		media_player.open_file(s.c_str());
		media_player.show();
	} else {
		media_player.hide();
		media_player.stop();
		SDL_Color red = { 255, 0, 0 };
		lp3::sdl::Surface surface
			= TTF_RenderText_Blended_Wrapped(
				font, "Can't handle this file type. :'(", red, 1920);
		if (static_cast<SDL_Surface *>(surface) == nullptr) {
			LP3_LOG_ERROR("Error running TTF_RenderText_Blended_Wrapped: %s", SDL_GetError());
			LP3_THROW2(lp3::core::Exception, "Error writing text.");
		}
		new_texture = SDL_CreateTextureFromSurface(renderer, surface);
	}
	if (new_texture) {
		if (this->texture) {
			SDL_DestroyTexture(this->texture);
		}
		this->texture = new_texture;
	}
}

void FileBrowser::update(Controls & controls) {
	if (!file_is_open) {
		update_directory_browser(controls);
	} else {
		update_media_viewer(controls);
	}
}

void FileBrowser::update_directory_browser(Controls & controls) {
	if (index > 0 && controls.up()) {
		--index;
	}
	if (index < (max - 1) && controls.down()) {
		++index;
	}
	if (controls.accept()) {
		if (index < lp3::narrow<long long>(current_folders.size())) {
			const auto new_path
				= fs::path(current_directory) / fs::path(current_folders[index]);
			const auto abs_new_path = fs::absolute(new_path);
			set_directory(abs_new_path);
			old_index = -1; // trigger redraw
		}
		else if (index < lp3::narrow<long long>(
			current_folders.size() + current_files.size())) {
			set_file();
			return;
		}
	}
	if (controls.cancel()) {
		auto current = fs::absolute(fs::path(current_directory));
		if (top_directory != current_directory) {
			auto new_path
				= fs::path(current_directory).parent_path();
			set_directory(fs::absolute(new_path));
			old_index = -1; // trigger redraw
		}
	}
}

void FileBrowser::update_media_viewer(Controls & controls) {
	if (controls.cancel()) {
		this->file_is_open = false;
		old_index = -1; // trigger redraw
		media_player.hide();
		media_player.stop();
	}
	if (index > current_folders.size() && controls.left()) {
		--index;
		set_file();
	}
	if (index < (max - 1) && controls.right()) {
		++index;
		set_file();
	}
}

void FileBrowser::update_text() {
	LP3_LOG_INFO("update_text()")
	std::stringstream text;
	constexpr int max_visible_lines = 15;

	std::int64_t start_at = index - 7;
	if (start_at < 0) {
		start_at = 0;
	}
	std::int64_t end_at = start_at + max_visible_lines;
	if (end_at > max) {
		if (max <= max_visible_lines) {
			start_at = 0;
			end_at = max;
		} else {
			start_at -= (end_at - max);
			// end_at = max_visible_lines;
		}
	}

	int another_index = 0;
	auto iterate_list = [&](std::vector<fs::path> & list) {
		for (const auto & s : list) {
			if (another_index >= start_at && another_index < end_at) {
				text << (index == another_index ? "-->" : "   ") << s << "\n";
			}
			++another_index;
		}
	};
	iterate_list(current_folders);
	iterate_list(current_files);

	auto text_s = text.str();

	SDL_Color yellow = { 255, 255, 0 };
	if (text_s.length() <= 0) {
		text_s = " * * empty directory * *";
	}
	lp3::sdl::Surface surface
		= TTF_RenderText_Blended_Wrapped(font, text_s.c_str(), yellow, 1920);
	if (static_cast<SDL_Surface *>(surface) == nullptr) {
		LP3_LOG_ERROR("Error running TTF_RenderText_Blended_Wrapped: %s", SDL_GetError());
		LP3_THROW2(lp3::core::Exception, "Error writing text.");
	}
	auto new_texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (this->texture) {
		SDL_DestroyTexture(this->texture);
	}
	this->texture = new_texture;

	this->rect = SDL_Rect{};
	this->rect->w = static_cast<SDL_Surface *>(surface)->w;
	this->rect->h = static_cast<SDL_Surface *>(surface)->h;
	this->rect->x = 0;
	this->rect->x = 0;
}

}   }
