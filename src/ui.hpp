#ifndef UI_H
#define UI_H

#include <array>
#include "settings.hpp"
#include "multipga.hpp"

namespace ui
{
/**
 * @brief Handle user input and update settings based on interaction.
 * 
 * @param is_sampling is a reference to a boolean, which manages our device state. Messages may or may not have an effect when in a particular state.
 * @param buffer_rx is a reference to a std::array of type uint8_t and length 64 (the incoming USB packet).
 * @param settings is a reference to an instantiation of the Settings struct.
 */
void handleInput(bool &is_sampling, std::array<uint8_t, 64> &buffer_rx, Settings &settings, MultiPGA &multi_pga);
};

#endif
