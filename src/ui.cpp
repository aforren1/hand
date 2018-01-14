#include <array>
#include "ui.hpp"
#include "settings.hpp"
#include "calibration.hpp"

void ui::handleInput(bool &is_sampling, std::array<uint8_t, 64> &buffer_rx, Settings &settings)
{
    if (is_sampling)
    {
        if (buffer_rx[0] == 's') // change to settings mode
        {
            is_sampling = false;
        }
    }
    else // dealing with settings
    {
        if (buffer_rx[0] == 's') // set
        {

        }
        else if (buffer_rx[0] == 'g') // get
        {

        }
        else if (buffer_rx[0] == 'a') // change to acquire mode
        {
            calibration::calibrateAllChannels(settings.pga_settings);
            is_sampling = true;
        }
    }
}
