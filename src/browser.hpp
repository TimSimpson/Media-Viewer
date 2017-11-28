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
    FileBrowser(lp3::core::MediaManager & media, lp3::sdl::Renderer & renderer);
	~FileBrowser();

    void render();

	void update(Controls & controls);

private:
    lp3::core::MediaManager & media;
	lp3::sdl::Renderer & renderer;
	SDL_Texture * texture;
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
