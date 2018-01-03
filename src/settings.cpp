#include "settings.hpp"
#include <algorithm>

Settings::Settings(float sampling_frequency, bool game, bool verbose) {
    setSamplingFrequency(sampling_frequency);
    game_mode = game;
    verbosity = verbose;
}

int Settings::setSamplingFrequency(float sampling_frequency) {
    if (sampling_frequency > 1000 || sampling_frequency < 1) {
        return 1;
    }
    sampling_frequency_hz = sampling_frequency;
    sampling_period_s = 1/sampling_frequency_hz;
    sampling_period_us = sampling_period_s * 1000000;
    return 0;
}

float Settings::getSamplingFrequency() {
    return sampling_frequency_hz;
}

int Settings::setGameMode(bool game) {
    game_mode = game;
    return 0;
}

bool Settings::getGameMode() {
    return game_mode;
}

int Settings::setVerbosity(bool verbose) {
    verbosity = verbose;
    return 0;
}

bool Settings::getVerbosity() {
    return verbosity;
}

int Settings::setGain(int finger, int channel, int slot, float value) {
    // check indexing
    if (finger < -1 || finger > 4 || channel < -1 || finger > 3 || slot < 0 || slot > 3) {
            return 1;
        }
    // check boundaries
    if (value < gain_settings.lower_bounds[slot] || value > gain_settings.upper_bounds[slot]) {
        return 1;
    }
    if (finger == -1 && channel == -1) { // apply setting to all channels, all fingers
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                gain_settings.gains[i][j][slot] = value;
                gain_settings.updateProduct(i, j, slot);
            }
        }
    } else if (finger == -1) { // apply setting to single channel, all fingers
        for (int i = 0; i < 4; i++) {
            gain_settings.gains[i][channel][slot] = value;
            gain_settings.updateProduct(i, channel, slot);
        }
    } else if (channel == -1) { // apply setting to all channels, single finger
        for (int j = 0; j < 3; j++) {
            gain_settings.gains[finger][j][slot] = value;
            gain_settings.updateProduct(finger, j, slot);
        }
    } else { // apply setting to single channel, single finger
        gain_settings.gains[finger][channel][slot] = value;
        gain_settings.updateProduct(finger, channel, slot);
    }
    return 0;
}

float Settings::getGain(int finger, int channel, int slot) {
    return gain_settings.gains[finger][channel][slot];
}

GainSettings::GainSettings() {
    for (int i = 0; i < 5; i++) { // iterate over fingers
        for (int j = 0; j < 4; j++) { // over channels
            gains[i][j][0] = 32; // front gain
            gains[i][j][1] = 1;  // fine gain
            gains[i][j][2] = 9;  // output gain
            gains[i][j][3] = 32 * 9; // product
        }
    }
    lower_bounds = {{0, 0, 0, 0}};
    upper_bounds = {{128, 1, 100, 12800}}; // really not sure about 100 and 12800
}

void GainSettings::updateProduct(int finger, int channel, int slot) {
    if (slot == 3) { // set the product, so use lookup table to update other elements
        // 
        float small_val = 0; // lower limit for the fine gain
        float large_val = 1; // upper limit for the fine gain
        float front_val;
        float base_gain = gains[finger][channel][3]/gains[finger][channel][2];
        if (base_gain <= 4.1) { front_val = 4; }
        else if (base_gain <= 8.1) { front_val = 8; }
        else if (base_gain <= 16.1) { front_val = 16; }
        else if (base_gain <= 23.37) { front_val = 23.27; }
        else if (base_gain <= 32.1) { front_val = 32; }
        else if (base_gain <= 42.77) { front_val = 42.67; }
        else if (base_gain <= 64.1) { front_val = 64; }
        else { front_val = 128; }
        gains[finger][channel][0] = front_val;
        // restrict to [0, 1]
        gains[finger][channel][1] = std::min(std::max(base_gain/front_val, small_val), large_val);
    } else { // set one of the components, so recompute the product
        gains[finger][channel][3] = gains[finger][channel][0] * 
                                    gains[finger][channel][1] * 
                                    gains[finger][channel][2];
    }
}