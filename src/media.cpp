#include "media.hpp"

#include <wmp.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>

namespace st4 { namespace media_viewer {

// Makes this an ATL program, which requires a singleton... thing.
// This is because the Media Player control is incredibly old.
// See these docs for more info.
// https://groups.google.com/forum/#!topic/microsoft.public.vc.atl/QdtwKxBNNJ8
class CustomModule : public CAtlExeModuleT<CustomModule> {};

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


MediaPlayer::MediaPlayer(lp3::sdl::Window & window)
:   impl(nullptr)
{

}

MediaPlayer::~MediaPlayer() {
}


}   }
