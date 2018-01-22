#include <array>
#include "ui.hpp"
#include "settings.hpp"
#include "calibration.hpp"
#include "packing.hpp"
#include "constants.hpp"

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
        if (buffer_rx[0] == 'c') // config
        {
            if (buffer_rx[1] == 'f') // sampling frequency
            {
                std::array<uint8_t, 4> flt_container;
                std::copy_n(buffer_rx.begin() + 2, 4, flt_container.begin());
                float freq = packing::bigendbytes2num<float>(flt_container);
                int res = settings.setSamplingFrequency(freq);
                // store res as last error
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                uint8_t val = buffer_rx[2];
                int res = settings.setGameMode(val);
            }
            else if (buffer_rx[1] == 'v') // verbosity
            {
                uint8_t val = buffer_rx[2];
                int res = settings.setVerbosity(val);
            }
            else if (buffer_rx[1] == 'g') // gain
            {
                // first 2 bytes are the finger (-1 for all fingers)
                // next 2 are the channel (-1 for all channels)
                // next 2 are the slot (0 = front gain, 1 = fine, 2 = output, 3 = product)
                // next 4 are the float value
                std::array<uint8_t, 4> flt_container;
                int8_t finger = buffer_rx[2];
                int8_t channel = buffer_rx[3];
                int8_t slot = buffer_rx[4];
                std::copy_n(buffer_rx.begin() + 5, 4, flt_container.begin());
                float val = packing::bigendbytes2num<float>(flt_container);
                int res = settings.setGain(finger, channel, slot, val);
            }
        }
        else if (buffer_rx[0] == 'g') // get
        {
            if (buffer_rx[1] == 'f') // sampling frequency
            {
                float freq = settings.getSamplingFrequency();
                // pack & send packet (prepend i for info?)
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                bool mode = settings.getGameMode();
                // TODO: Send result
            }
            else if (buffer_rx[1] == 'v') // verbosity
            {
                bool verbosity = settings.getVerbosity();
            }
            else if (buffer_rx[1] == 'g') // gain
            {
                int8_t finger = buffer_rx[2];
                int8_t channel = buffer_rx[3];
                int8_t slot = buffer_rx[4];
                float gain = settings.getGain(finger, channel, slot);
            }
        }
        else if (buffer_rx[0] == 'a') // change to acquire mode
        {
            // try a "smart" recalibration -- only touch channels which have channged
            for (std::size_t i = 0; i < settings.pga_settings.gains_and_offsets.size(); i++)
            {
                for (std::size_t j = 0; j < settings.pga_settings.gains_and_offsets[i].size(); j++)
                {
                    if (settings.pga_settings.gains_and_offsets[i][j] != settings.pga_settings_copy.gains_and_offsets[i][j])
                    {
                        calibration::calibrateChannel(constants::pin::sensor_pins_nested[i][j],
                                                      settings.pga_settings.gains_and_offsets[i][j]);
                        settings.pga_settings_copy.gains_and_offsets[i][j] = settings.pga_settings.gains_and_offsets[i][j];
                    }
                }
            }
            // calibration::calibrateAllChannels(settings.pga_settings);
            is_sampling = true;
        }
    }
}
