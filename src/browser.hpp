#ifndef FILE_ST4_BROWSER_HPP
#define FILE_ST4_BROWSER_HPP
#pragma once

#include <lp3/core.hpp>
#include <SDL_ttf.h>
#include "controls.hpp"

namespace st4 { namespace media_viewer {

// FileBrowser
// =======
// This class browses a file or directory.
class FileBrowser {
public:
    FileBrowser(lp3::core::MediaManager & media, lp3::sdl::Renderer & renderer,
		        std::string top_directory);
	~FileBrowser();

    void render();

	boost::optional<std::string> update(Controls & controls);

private:
    lp3::core::MediaManager & media;
	lp3::sdl::Renderer & renderer;
	std::string top_directory;
	SDL_Texture * texture;
	lp3::sdl::RWops font_file;
	TTF_Font * font;
	SDL_Rect rect;
	long long index;
	long long old_index;
	long long max;
	std::string current_directory;
	std::vector<std::string> current_folders;
	std::vector<std::string> current_files;
	void set_directory(const std::string & path);
	void update_text();
};

}   }

#endif
