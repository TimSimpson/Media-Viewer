#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include "media.hpp"
#include <lp3/main.hpp>

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace sdl = lp3::sdl;



#define WIN_CALL(code) { \
		HRESULT	result = code ;  \
		if (result < 0) { \
			LP3_LOG_ERROR("Error at %s, %d", __FILE__, __LINE__); \
			return 1;	\
		} \
	}


using namespace st4::media_viewer;


core::GlobalVar<AtlModule> atl_module;


int _main(core::PlatformLoop & loop) {
	core::LogSystem log;
    sdl::SDL2 sdl2(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK| SDL_INIT_GAMECONTROLLER);
    gfx::Window window("Media Viewy", glm::vec2{ 427, 240 });


	AtlHostingCode atl_hosting_code;
	if (!atl_hosting_code.ok()) {
		LP3_LOG_ERROR("Couldn't init ATL...");
		return 1;
	}

    MediaPlayer media_player(window.sdl_window());

    media_player.hide_bar();
    media_player.open_file("D:\\Work\\media.mp4");


    return loop.run([&]() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				return false;
			case SDL_WINDOWEVENT:
				media_player.handle_events(e.window);
				break;
			default:
				break;
			}
        }
		return true;
    });
}

LP3_MAIN(_main)
