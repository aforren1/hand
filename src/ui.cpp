#include <array>
#include <algorithm>
#include "ui.hpp"
#include "settings.hpp"
#include "calibration.hpp"
#include "packing.hpp"
#include "constants.hpp"
#include "multipga.hpp"

#if USB_RAWHID
#include "hid_comm.hpp"
#else
#include "serial_comm.hpp"
#endif

int last_err = 0;

void ui::handleInput(bool &is_sampling, std::array<uint8_t, 64> &buffer_rx, std::array<uint8_t, 64> &buffer_tx,
                     Settings &settings, MultiPGA &multi_pga)
{
    if (is_sampling)
    {
        if (buffer_rx[0] == 'c') // change to config mode
        {
            is_sampling = false;
        }
    }
    else // dealing with settings
    {
        if (buffer_rx[0] == 's') // set
        {
            if (buffer_rx[1] == 'f') // sampling frequency
            {
                std::array<uint8_t, 4> flt_container;
                std::copy_n(buffer_rx.begin() + 2, 4, flt_container.begin());
                float freq = packing::bigendbytes2num<float>(flt_container);
                last_err = settings.setSamplingFrequency(freq);
                // store res as last error
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                uint8_t val = buffer_rx[2];
                last_err = settings.setGameMode(val);
            }
            else if (buffer_rx[1] == 'v') // verbosity
            {
                uint8_t val = buffer_rx[2];
                last_err = settings.setVerbosity(val);
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
                last_err = settings.setGain(finger, channel, slot, val);
            }
        }
        else if (buffer_rx[0] == 'g') // get
        {
            if (buffer_rx[1] == 'f') // sampling frequency
            {
                float freq = settings.getSamplingFrequency();
                std::array<uint8_t, 4> flt_container;
                packing::num2bigendbytes(freq, flt_container);
                std::copy_n(flt_container.begin(), 4, buffer_tx.begin());
                communication::sendInfo(buffer_tx);
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                bool mode = settings.getGameMode();
                buffer_tx[0] = mode;
                communication::sendInfo(buffer_tx);
            }
            else if (buffer_rx[1] == 'v') // verbosity
            {
                bool verbosity = settings.getVerbosity();
                buffer_tx[0] = verbosity;
                communication::sendInfo(buffer_tx);
            }
            else if (buffer_rx[1] == 'g') // gain
            {
                int8_t finger = buffer_rx[2];
                int8_t channel = buffer_rx[3];
                int8_t slot = buffer_rx[4];
                float gain = settings.getGain(finger, channel, slot);
                std::array<uint8_t, 4> flt_container;
                packing::num2bigendbytes(gain, flt_container);
                std::copy_n(flt_container.begin(), 4, buffer_tx.begin());
                communication::sendInfo(buffer_tx);
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
#ifndef NOHARDWARE
                        calibration::calibrateChannel(constants::pin::sensor_pins_nested[i][j],
                                                      settings.pga_settings.gains_and_offsets[i][j],
                                                      multi_pga);
#endif
                        settings.pga_settings_copy.gains_and_offsets[i][j] = settings.pga_settings.gains_and_offsets[i][j];
                    }
                }
            }
            // calibration::calibrateAllChannels(settings.pga_settings);
            is_sampling = true;
        }
    }
}
