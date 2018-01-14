#ifndef UI_H
#define UI_H

#include <array>
#include "settings.hpp"

namespace ui 
{
    void handleInput(bool &is_sampling, std::array<uint8_t, 64> &buffer_rx, Settings &settings);
};

#endif
