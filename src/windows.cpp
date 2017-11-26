#include <lp3/core.hpp>
#include <lp3/gfx.hpp>

#include "wmp.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
#include <SDL_syswm.h>
#include "media.hpp"
#include <lp3/main.hpp>

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace sdl = lp3::sdl;

// // Makes this an ATL program, which requires a singleton... thing.
// // This is because the Media Player control is incredibly old.
// // See these docs for more info.
// // https://groups.google.com/forum/#!topic/microsoft.public.vc.atl/QdtwKxBNNJ8
// class CustomModule : public CAtlExeModuleT<CustomModule> {};
// core::GlobalVar<CustomModule> _Module;

// More hot Media Player docs:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd564579(v=vs.85).aspx





#define WIN_CALL(code) { \
		HRESULT	result = code ;  \
		if (result < 0) { \
			LP3_LOG_ERROR("Error at %s, %d", __FILE__, __LINE__); \
			return 1;	\
		} \
	}



// class AtlHostingCode {
// public:
// 	AtlHostingCode() {
// 		result = AtlAxWinInit();
// 	}

// 	~AtlHostingCode() {
// 		if (result) {
// 			AtlAxWinTerm();
// 		}
// 	}

// 	bool ok() const { return result; }
// private:
// 	bool result;
// };

using namespace st4::media_viewer;


core::GlobalVar<AtlModule> atl_module;


int _main(core::PlatformLoop & loop) {
	core::LogSystem log;
    sdl::SDL2 sdl2(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK| SDL_INIT_GAMECONTROLLER);
    gfx::Window window("Media Viewy", glm::vec2{ 427, 240 });

	// Dig out the HWND from SDL.
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(window.sdl_window(), &info)) {
		LP3_LOG_ERROR("SDL ERROR: %s", SDL_GetError());
		return 1;
	}
	HWND h_wnd = info.info.win.window;

	AtlHostingCode atl_hosting_code;
	if (!atl_hosting_code.ok()) {
		LP3_LOG_ERROR("Couldn't init ATL...");
		return 1;
	}

    // ActiveX's window.
	CAxWindow  ax_window;
	//CComPtr<IWMPPlayer>  m_spWMPPlayer;  // Smart pointer to IWMPPlayer interface.

	CComPtr<IAxWinHostWindow>  ax_host_window;

	RECT client_rect;
	LP3_ASSERT(GetClientRect(h_wnd, &client_rect));

	ax_window.Create(
        h_wnd, client_rect, nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	WIN_CALL(ax_window.QueryHost(&ax_host_window));


	WIN_CALL(ax_host_window->CreateControl(
        CComBSTR(_T("{6BF52A52-394A-11d3-B153-00C04F79FAA6}")), ax_window, 0));

	CComPtr<IWMPPlayer> media_player;
	WIN_CALL(ax_window.QueryControl(&media_player));

	//IWMPMedia media;
	//media_player->put_currentMedia(media);

	media_player->put_fullScreen(VARIANT_TRUE);
	CComBSTR ui_mode("none");
	media_player->put_uiMode(ui_mode);

	CComBSTR path("D:\\Work\\media.mp4");
	media_player->put_URL(path);

    return loop.run([&]() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				return false;
			case SDL_WINDOWEVENT:
				switch (e.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					ax_window.ResizeClient(e.window.data1, e.window.data2, true);
				default:
					break;
				}
				break;
			default:
				break;
			}
        }
		return true;
    });
}

LP3_MAIN(_main)
