#include "browser.hpp"
#include <SDL_ttf.h>
#include <filesystem>
#include <sstream>

namespace fs = std::experimental::filesystem;

namespace st4 { namespace media_viewer {

FileBrowser::FileBrowser(
    lp3::core::MediaManager & _media, lp3::sdl::Renderer & _renderer)
:   media(_media),
	renderer(_renderer),
	texture(),
	font(nullptr),
	rect(),
	index(-1),
	old_index(-1),
	max(0),
	current_directory(),
	current_folders(),
	current_files()
{
    auto input_file = media.load("fonts/Inconsolata-Regular.ttf");
	this->font = TTF_OpenFontRW(input_file, 0, 56);
    if (nullptr == font) {
        LP3_LOG_ERROR(SDL_GetError());
        LP3_THROW2(lp3::core::Exception, "Error loading font!");
    }

	this->set_directory(media.path("/"));
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

void FileBrowser::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void FileBrowser::set_directory(const std::string & path) {
	current_directory = path;
	current_folders.clear();
	current_files.clear();
	index = 0;

	auto root = fs::absolute(fs::path(path));
	for (auto itr = fs::directory_iterator(root);
		itr != fs::directory_iterator{}; ++itr) {
		if (is_regular_file(*itr)) {
			current_files.push_back(itr->path().filename().string());
		}
		else {
			current_folders.push_back(itr->path().filename().string());
		}
	}
	this->max = current_folders.size() + current_files.size();
}

void FileBrowser::update(Controls & controls) {
	if (index > 0 && controls.up()) {
		-- index;
	}
	if (index <  max -1 && controls.down()) {
		++ index;
	}
	if (old_index != index) {
		update_text();
		old_index = index;
	}
}

void FileBrowser::update_text() {
	std::stringstream text;
	int another_index = 0;
	auto iterate_list = [&](std::vector<std::string> & list) {
		for (const auto & s : list) {
			text << (index == another_index ? "-->" : "   ") << s << "\n";
			++another_index;
		}
	};
	iterate_list(current_folders);
	iterate_list(current_files);

	auto text_s = text.str();

	SDL_Color yellow = { 255, 255, 0 };
	// lp3::sdl::Surface surface = TTF_RenderText_Solid(font, text_s.c_str(), yellow);
	lp3::sdl::Surface surface 
		= TTF_RenderText_Blended_Wrapped(font, text_s.c_str(), yellow, 1920);
	auto new_texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (this->texture) {
		SDL_DestroyTexture(this->texture);
	}
	this->texture = new_texture;
	

	this->rect.w = static_cast<SDL_Surface *>(surface)->w;
	this->rect.h = static_cast<SDL_Surface *>(surface)->h;
	this->rect.x = 0;
	this->rect.x = 0;
}

}   }
