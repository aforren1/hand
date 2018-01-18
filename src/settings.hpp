#ifndef SETTINGS_H
#define SETTINGS_H

#include <array>

struct PGASettings
{
    // first dim is finger, second is slot, 3rd is value (front gain, fine gain, output gain, coarse offset, fine offset)
    std::array<std::array<std::array<float, 6>, 4>, 5> gains_and_offsets; ///< 5*4*6 nested array of floats. First dim is finger, second is channel, third is slot.
    std::array<float, 6> lower_bounds;
    std::array<float, 6> upper_bounds;
    PGASettings();
    void updateProduct(int finger, int channel, int slot);
};

struct Settings
{
    float sampling_frequency_hz;
    float sampling_period_s;
    unsigned long sampling_period_us; ///< sampling_period_s * 1000000
    bool game_mode;                   ///< Whether to precompute the x,y,z per finger
    bool verbosity;                   ///< Whether to push messages during calibration, etc.
    PGASettings pga_settings;         ///< Manages the op-amp settings
    PGASettings pga_settings_copy;    ///< Copy of the settings, used to infer which channels to recalibrate
    // TODO: add ADC averaging, resolution to settings?

    Settings(float, bool, bool); // sampling frequency in hz, game mode, and verbosity
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
