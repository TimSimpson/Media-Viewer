#ifndef FILE_ST4_BROWSER_HPP
#define FILE_ST4_BROWSER_HPP
#pragma once

#include <lp3/core.hpp>

namespace st4 { namespace media_viewer {

// FileBrowser
// =======
// This class browses a file or directory.
class FileBrowser {
public:
    FileBrowser(lp3::core::MediaManager & media, lp3::sdl::Renderer & renderer);

    void render();
private:
    lp3::core::MediaManager & media;
	lp3::sdl::Renderer & renderer;
	lp3::sdl::Texture texture;
};

}   }

#endif
