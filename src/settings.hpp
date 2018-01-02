#ifndef SETTINGS_H
#define SETTINGS_H

#include <array>

struct GainSettings {
    // first dim is finger, second is slot, 3rd is value
    std::array<std::array<std::array<float, 4>, 4>, 5> gains;
    std::array<float, 4> lower_bounds;
    std::array<float, 4> upper_bounds;
    GainSettings ();
    void updateProduct (int finger, int channel, int slot);
};

struct Settings {
    float sampling_frequency_hz;
    float sampling_period_s;
    unsigned long sampling_period_us; // sampling_period_s * 1000000
    bool game_mode; /// Whether to precompute the x,y,z per finger
    bool verbosity; /// Whether to push messages during calibration, etc.
    GainSettings gain_settings; /// Manages the op-amp settings
    // TODO: add ADC averaging, resolution to settings?

    Settings (float, bool, bool); // sampling frequency in hz, game mode, and verbosity
    int setSamplingFrequency(float);
    float getSamplingFrequency();
    int setGameMode(bool);
    bool getGameMode();
    int setVerbosity(bool);
    bool getVerbosity();
    int setGain(int, int, int, float); // finger, channel, slot, value
    float getGain(int, int, int);

};

#endif
