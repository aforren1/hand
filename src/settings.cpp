#include <algorithm>
#include "constants.hpp"
#include "settings.hpp"

Settings::Settings(float sampling_frequency, bool game)
{
    setSamplingFrequency(sampling_frequency);
    game_mode = game;
}

uint8_t Settings::setSamplingFrequency(float sampling_frequency)
{
    if (sampling_frequency > 1000 || sampling_frequency <= 0.01) // current sampling frequency maxes out at 1kHz (hardware & communication restriction), and cannot be 0 or negative
    {
        return 1;
    }
    sampling_frequency_hz = sampling_frequency;
    sampling_period_s = 1 / sampling_frequency_hz;
    sampling_period_us = sampling_period_s * 1000000; // note that any trailing decimals are removed at this point (casting to int rounds toward 0)
    return 0;
}

float Settings::getSamplingFrequency()
{
    return sampling_frequency_hz;
}

uint8_t Settings::setGameMode(bool game)
{
    game_mode = game;
    return 0;
}

bool Settings::getGameMode()
{
    return game_mode;
}

uint8_t Settings::setGain(int8_t finger, int8_t channel, int8_t stage, float value)
{
    // check indexing
    // Note that any channel other than -1 is disallowed; in the future, it *could* be possible
    // to allow different gains per channel, but I think the practicality is limited.
    // TODO: Should we nix allowing channel to be used ever?
    if (finger < -1 || finger > 4 || channel < -1 || finger > 3 || stage < 0 || stage > 3)
    {
        return 1;
    }
    // check boundaries
    if (value < pga_settings.lower_bounds[stage] || value > pga_settings.upper_bounds[stage])
    {
        return 1;
    }
    if (finger == -1 && channel == -1)
    { // apply setting to all channels, all fingers
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                pga_settings.gains_and_offsets[i][j][stage] = value;
                pga_settings.updateProduct(i, j, stage);
            }
        }
    }
    else if (finger == -1)
    { // apply setting to single channel, all fingers
        for (int i = 0; i < 4; i++)
        {
            pga_settings.gains_and_offsets[i][channel][stage] = value;
            pga_settings.updateProduct(i, channel, stage);
        }
    }
    else if (channel == -1)
    { // apply setting to all channels, single finger
        for (int j = 0; j < 3; j++)
        {
            pga_settings.gains_and_offsets[finger][j][stage] = value;
            pga_settings.updateProduct(finger, j, stage);
        }
    }
    else
    { // apply setting to single channel, single finger
        pga_settings.gains_and_offsets[finger][channel][stage] = value;
        pga_settings.updateProduct(finger, channel, stage);
    }
    return 0;
}

float Settings::getGain(int8_t finger, int8_t channel, int8_t stage)
{
    return pga_settings.gains_and_offsets[finger][channel][stage];
}

PGASettings::PGASettings()
{
    for (int i = 0; i < 5; i++)
    { // iterate over fingers
        for (int j = 0; j < 4; j++)
        {                                                                     // over channels
            gains_and_offsets[i][j][0] = 32;                                  // front gain
            gains_and_offsets[i][j][1] = 1;                                   // fine gain
            gains_and_offsets[i][j][2] = 9;                                   // output gain
            gains_and_offsets[i][j][3] = 32 * 9;                              // product
            gains_and_offsets[i][j][4] = 0;                                   // coarse offset (TODO: fix)
            gains_and_offsets[i][j][5] = 0.25 * constants::calibration::mvcc; // fine offset (TODO: fix)
        }
    }
    lower_bounds = {{0, 0, 0, 0, -constants::calibration::coarse_max, 0}};
    upper_bounds = {{128, 1, 9, 128 * 9, constants::calibration::coarse_max, constants::calibration::mvcc}};
}

void PGASettings::updateProduct(int8_t finger, int8_t channel, int8_t stage)
{
    // http://www.ti.com/lit/ug/sbou024b/sbou024b.pdf, pg 109
    if (stage == 3)
    { // set the product, so use lookup table to update other elements
        //
        float small_val = 0; // lower limit for the fine gain
        float large_val = 1; // upper limit for the fine gain
        float front_val;
        float base_gain = gains_and_offsets[finger][channel][3] / gains_and_offsets[finger][channel][2];
        if (base_gain <= 4.1)
        {
            front_val = 4;
        }
        else if (base_gain <= 8.1)
        {
            front_val = 8;
        }
        else if (base_gain <= 16.1)
        {
            front_val = 16;
        }
        else if (base_gain <= 23.37)
        {
            front_val = 23.27;
        }
        else if (base_gain <= 32.1)
        {
            front_val = 32;
        }
        else if (base_gain <= 42.77)
        {
            front_val = 42.67;
        }
        else if (base_gain <= 64.1)
        {
            front_val = 64;
        }
        else
        {
            front_val = 128;
        }
        gains_and_offsets[finger][channel][0] = front_val;
        // restrict the fine gain to [0, 1]
        gains_and_offsets[finger][channel][1] = std::max(std::min(base_gain / front_val, large_val), small_val); // note to future: ran into funky std::min/max here
    }
    else
    { // set one of the components, so recompute the product
        gains_and_offsets[finger][channel][3] = gains_and_offsets[finger][channel][0] *
                                                gains_and_offsets[finger][channel][1] *
                                                gains_and_offsets[finger][channel][2];
    }
}
