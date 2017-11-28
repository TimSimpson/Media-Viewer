#include "browser.hpp"
#include <SDL_ttf.h>

namespace st4 { namespace media_viewer {

FileBrowser::FileBrowser(
    lp3::core::MediaManager & _media, lp3::sdl::Renderer & _renderer)
:   media(_media),
	renderer(_renderer)
{
    auto input_file = media.load("Inconsolata-Regular.ttf");
    auto font = TTF_OpenFontRW(input_file, 0, 16);
    if (nullptr == font) {
        LP3_LOG_ERROR(SDL_GetError());
        LP3_THROW2(lp3::core::Exception, "Error loading font!");
    }

    SDL_Color yellow = {255, 255, 0};

    lp3::sdl::Surface surface = TTF_RenderText_Solid(font, "HELLO WORLD", yellow);
    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void FileBrowser::render() {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 100;
    rect.h = 100;
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

}   }
