#include <array>
#include <algorithm>
#include "ui.hpp"
#include "settings.hpp"
#include "calibration.hpp"
#include "packing.hpp"
#include "constants.hpp"
#include "multipga.hpp"
#include "analog.hpp"
#include "comm.hpp"

uint8_t last_err_code = 0;

void ui::handleInput(bool &is_sampling, std::array<uint8_t, 64> &buffer_rx, Settings &settings)
{
    std::array<uint8_t, 64> buffer_tx;
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
                last_err_code = settings.setSamplingFrequency(freq);
                // store res as last error
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                uint8_t val = buffer_rx[2];
                last_err_code = settings.setGameMode(val);
            }
            else if (buffer_rx[1] == 'g') // gain
            {
                // first byte is the finger (-1 for all fingers)
                // next byte is the channel (-1 for all channels)
                // next byte is the slot (0 = front gain, 1 = fine, 2 = output, 3 = product)
                // next 4 are the float value
                std::array<uint8_t, 4> flt_container;
                int8_t finger = buffer_rx[2];
                int8_t channel = buffer_rx[3];
                int8_t slot = buffer_rx[4];
                std::copy_n(buffer_rx.begin() + 5, 4, flt_container.begin());
                float val = packing::bigendbytes2num<float>(flt_container);
                last_err_code = settings.setGain(finger, channel, slot, val);
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
                comm::sendRawPacket(buffer_tx);
            }
            else if (buffer_rx[1] == 'm') // game mode
            {
                bool mode = settings.getGameMode();
                buffer_tx[0] = mode;
                comm::sendRawPacket(buffer_tx);
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
                comm::sendRawPacket(buffer_tx);
            }
            else if (buffer_rx[1] == 'e') // last error
            {
                buffer_tx[0] = last_err_code;
                comm::sendRawPacket(buffer_tx);
            }
        }
        else if (buffer_rx[0] == 'a') // change to acquisition mode
        {
            // try a "smart" recalibration -- only touch channels which have channged
            for (std::size_t i = 0; i < settings.pga_settings.gains_and_offsets.size(); i++)
            {
                for (std::size_t j = 0; j < settings.pga_settings.gains_and_offsets[i].size(); j++)
                {
                    if (settings.pga_settings.gains_and_offsets[i][j] != settings.pga_settings_copy.gains_and_offsets[i][j])
                    {
#ifndef NOHARDWARE
                        calibration::calibrateChannel(constants::pin::mux_pins_nested[i][j],
                                                      settings.pga_settings.gains_and_offsets[i][j]);
#endif
                        settings.pga_settings_copy.gains_and_offsets[i][j] = settings.pga_settings.gains_and_offsets[i][j];
                    }
                }
            }
            // calibration::calibrateAllChannels(settings.pga_settings);
            is_sampling = true;
        }
        else if (buffer_rx[0] == 't') // Run calibration without moving to acquisition mode
        {
#ifndef NOHARDWARE
            calibration::calibrateAllChannels(settings.pga_settings);
#endif
            settings.pga_settings_copy = settings.pga_settings;
        }
        else if (buffer_rx[0] == 'd') // single data
        {
            std::array<uint16_t, 20> temp_data;
            analog::readAllOnce(temp_data);
            if (settings.getGameMode())
            {
                std::array<float, 15> temp_rot_data;
                analog::applyRotation(temp_data, temp_rot_data);
                comm::sendSample(temp_rot_data);
            }
            else
            {
                comm::sendSample(0, 0, temp_data);
            }
        }
        else if (buffer_rx[0] == 'e')
        {
            std::array<uint16_t, 20> temp_data;
            std::array<float, 5> temp_err_data;
            analog::readAllOnce(temp_data);
            analog::calcError(temp_data, temp_err_data);
            comm::sendSample(temp_err_data);
        }
    }
    buffer_rx.fill(0);
}
