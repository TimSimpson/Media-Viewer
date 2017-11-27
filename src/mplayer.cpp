#include "mplayer.hpp"

#include <wmp.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
#include <SDL_syswm.h>

namespace st4 { namespace media_viewer {

#define WIN_CALL(code) { \
        HRESULT result = code ;  \
        if (result < 0) { \
            LP3_LOG_ERROR("Error at %s, %d", __FILE__, __LINE__); \
            throw lp3::core::Exception();   \
        } \
    }

namespace {
    // Makes this an ATL program, which requires a singleton... thing.
    // This is because the Media Player control is incredibly old.
    // See these docs for more info.
    // https://groups.google.com/forum/#!topic/microsoft.public.vc.atl/QdtwKxBNNJ8
    class CustomModule : public CAtlExeModuleT<CustomModule> {};

    HWND get_hwnd_from_window(lp3::sdl::Window & window) {
        // Dig out the HWND from SDL.
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (!SDL_GetWindowWMInfo(window, &info)) {
            LP3_LOG_ERROR("SDL ERROR: %s", SDL_GetError());
            throw lp3::core::Exception();
        }
        return info.info.win.window;
    }
}

AtlModule::AtlModule()
:   module(new CustomModule())
{}

AtlModule::~AtlModule()
{
    delete module;
}


AtlHostingCode::AtlHostingCode()
:   result(AtlAxWinInit())
{
}

AtlHostingCode::~AtlHostingCode() {
    if (result) {
        AtlAxWinTerm();
    }
}


struct MediaPlayer::Impl {
    CAxWindow  ax_window;   // ActiveX's window.
    CComPtr<IAxWinHostWindow>  ax_host_window;
    CComPtr<IWMPPlayer> media_player;

    Impl(lp3::sdl::Window & window)
    :   ax_window(),
        ax_host_window(),
        media_player()
    {
        HWND h_wnd = get_hwnd_from_window(window);

        RECT client_rect;
        LP3_ASSERT(GetClientRect(h_wnd, &client_rect));

        ax_window.Create(
            h_wnd, client_rect, nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
        WIN_CALL(ax_window.QueryHost(&ax_host_window));

        WIN_CALL(ax_host_window->CreateControl(
            CComBSTR(_T("{6BF52A52-394A-11d3-B153-00C04F79FAA6}")),
            ax_window,
            0));


        WIN_CALL(ax_window.QueryControl(&media_player));

        media_player->put_fullScreen(VARIANT_TRUE);

    }

    ~Impl() {
		media_player->close();
		//ax_window.DestroyWindow();
    }
};

MediaPlayer::MediaPlayer(lp3::sdl::Window & window)
:	impl(new Impl(window))
{
}

MediaPlayer::~MediaPlayer() {
    delete impl;
}

void MediaPlayer::handle_events(const SDL_WindowEvent & window_event) {
	switch (window_event.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		impl->ax_window.ResizeClient(
			window_event.data1, window_event.data2, true);
	default:
		break;
	}
}
void MediaPlayer::hide_bar() {
    CComBSTR ui_mode("none");
    impl->media_player->put_uiMode(ui_mode);
}

void MediaPlayer::open_file(const char * path) {
    CComBSTR com_path(path);
    impl->media_player->put_URL(com_path);
}

}   }
