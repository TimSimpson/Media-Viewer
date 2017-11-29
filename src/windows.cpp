#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include <lp3/sims.hpp>
#include "browser.hpp"
#include "controls.hpp"
#include "mplayer.hpp"
#include <SDL_ttf.h>
#include <lp3/main.hpp>

// #include <filesystem>

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace sdl = lp3::sdl;
namespace sims = lp3::sims;


using namespace st4::media_viewer;


core::GlobalVar<AtlModule> atl_module;


int _main(core::PlatformLoop & loop) {
	core::LogSystem log;
	core::MediaManager media;

    sdl::SDL2 sdl2(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK| SDL_INIT_GAMECONTROLLER);
    TTF_Init();

	sdl::Window window = SDL_CreateWindow(
		"Media Viewer",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		427,
		240,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP
	);

	Controls controls;

	sdl::Renderer renderer
		= SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	std::string root_directory{ "D:\\Work\\MediaViewerFiles" };
	if (1 < loop.command_line_args().size()) {
		root_directory = loop.command_line_args()[1];
	}
	FileBrowser browser(window, media, renderer, root_directory);

	AtlHostingCode atl_hosting_code;
	if (!atl_hosting_code.ok()) {
		LP3_LOG_ERROR("Couldn't init ATL...");
		return 1;
	}


	int width, height;

	SDL_RenderSetLogicalSize(renderer, 1920, 1080);

	const std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps
	sims::GameClock clock(ms_per_update);

    return loop.run([&]() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				return false;
			case SDL_WINDOWEVENT:
				//media_player.handle_events(e.window);
				browser.handle_events(e.window);
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					width = e.window.data1;
					height = e.window.data2;
				}
				break;
			default:
				break;
			}
        }

		clock.run_updates([&](std::int64_t ms) {
			controls.update(ms);
			browser.update(controls);
		});

		SDL_RenderClear(renderer);
		browser.render();
		SDL_RenderPresent(renderer);
		return true;
    });
}

LP3_MAIN(_main)
