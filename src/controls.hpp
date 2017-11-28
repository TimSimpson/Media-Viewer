#ifndef FILE_ST4_CONTROLS_HPP
#define FILE_ST4_CONTROLS_HPP
#pragma once

#include <lp3/input.hpp>


namespace st4 { namespace media_viewer {

class Controls {
public:
    Controls();
    ~Controls();

    bool accept() const;

    bool cancel() const;

    bool down() const;

    bool left() const;

    bool right() const;

    bool up() const;

    void update(const std::int64_t ms);
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}   }

#endif
