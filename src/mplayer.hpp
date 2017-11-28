#ifndef FILE_ST4_MPLAYER_HPP
#define FILE_ST4_MPLAYER_HPP
#pragma once

#include <lp3/core.hpp>

namespace st4 { namespace media_viewer {


// This is an old school Windows handle thing for ATL code. It needs to exist
// before the app starts as a staticly defined variable somewhere.
class AtlModule {
public:
    AtlModule();
    ~AtlModule();
private:
    void * module;
};

// Sets up ATL's control hosting code. ATL is the madness used to host the
// Media Player control, meaning you need a single global instance of this
// in order to keep everything happy.
class AtlHostingCode {
public:
    AtlHostingCode();
    ~AtlHostingCode();
    inline bool ok() const { return result; }
private:
    bool result;
};


// Represents the good ole Windows Media Player.
class MediaPlayer {
public:
    MediaPlayer(lp3::sdl::Window & window);
    ~MediaPlayer();

	void handle_events(const SDL_WindowEvent & window_event);

	void hide(); 

    void hide_bar();

    void open_file(const char * path);
private:
    struct Impl;
    Impl * impl;
};

}   }

#endif
