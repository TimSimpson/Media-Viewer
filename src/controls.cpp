#include "controls.hpp"
#include <lp3/input.hpp>


namespace st4 { namespace media_viewer {

namespace input = lp3::input;

class Controls::Impl {
private:
    input::Controls controls;
    std::array<std::int64_t, input::max_button_count> back_off_time;
    std::array<bool, input::max_button_count> state;
public:
    Impl()
    :   controls()
    {
        input::PreferredButtonMapping game_pad_mapping;
        game_pad_mapping.device = input::PreferredDevice::GAME_PAD;
        game_pad_mapping.set_mapping(
            "D-pad Up", "D-pad Down", "D-pad Left", "D-pad Right",
            "A", "B", "Start");

        input::PreferredButtonMapping kb_mapping;
        kb_mapping.device = input::PreferredDevice::KEYBOARD;
        kb_mapping.set_mapping(
            "Up", "Down", "Left", "Right",
            "Z", "X", "Return");

        std::vector<input::PreferredButtonMapping> preferred_mappings = {
            game_pad_mapping, kb_mapping
        };

        controls.set_defaults(0, preferred_mappings);
    }

    bool accept() const {
        return state[4];
    }

    bool cancel() const {
        return state[5];
    }

    bool down() const {
        return state[1];
    }

    bool left() const {
        return state[2];
    }

    bool right() const {
        return state[3];
    }

    bool up() const {
        return state[0];
    }

    void update(const std::int64_t ms) {
		controls.update();

        // Each key has a repeat value which goes down here...
        for (std::int64_t & t : back_off_time) {
            if (t > 0) {
                t -= ms;
            }
        }

        // if the keys aren't down, the limit goes to zero right away.
        for (int i = 0; i < input::max_button_count; ++ i) {
            if (!controls.get_control(0).state(i)) {
                back_off_time[i] = 0;
            }
        }

        // Flip back on the states if necessary.
        for (int i = 0; i < input::max_button_count; ++ i) {
            if ((state[i] =
                    (controls.get_control(0).state(i) && back_off_time[i] <= 0))
                )
            {
                back_off_time[i] = 100;
            }
        }
    }
};

Controls::Controls()
:   impl(new Impl{})
{
}

Controls::~Controls() {
}

bool Controls::accept() const {
    return impl->accept();
}

bool Controls::cancel() const {
    return impl->cancel();
}

bool Controls::down() const {
    return impl->down();
}

bool Controls::left() const {
    return impl->left();
}

bool Controls::right() const {
    return impl->right();
}

bool Controls::up() const {
    return impl->up();
}

void Controls::update(const std::int64_t ms) {
    return impl->update(ms);
}



}   }
