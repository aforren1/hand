#ifndef SETTINGS_H
#define SETTINGS_H

#include <array>

struct PGASettings
{
    // first dim is finger, second is slot, 3rd is value (front gain, fine gain, output gain, coarse offset, fine offset)
    std::array<std::array<std::array<float, 6>, 4>, 5> gains_and_offsets; ///< 5*4*6 nested array of floats. First dim is finger, second is channel, third is slot.
    std::array<float, 6> lower_bounds;                                    ///< Lower bounds for the gain and offset settings.
    std::array<float, 6> upper_bounds;                                    ///< Upper bounds for the gain and offset settings.
    /**
     * @brief constructor for the PGASettings class.
     * 
     * @note This is an internal struct for managing the gain and offset settings for all PGAs.
     * @todo This is a bit clunky, and any suggestions for managing this info would be appreciated (should they be managed with the PGA stuff?)
     */
    PGASettings();
    /**
     * @brief Update the product of gains.
     * 
     * @note If the product has been updated, then all other gain settings will be updated.
     *       If one of the other gain settings has been updated (e.g. the fine gain), the product will be recalculated.
     */
    void updateProduct(int8_t finger, int8_t channel, int8_t slot);
};

struct Settings
{
    float sampling_frequency_hz;      ///< sampling_frequency_hz can take values (0, 1000]
    float sampling_period_s;          ///< sampling_period_s is 1 / sampling_frequency_hz
    unsigned long sampling_period_us; ///< sampling_period_us is sampling_period_s * 1000000, the sampling period in microseconds
    bool game_mode;                   ///< Whether to precompute the x, y, z per finger. Note that this changes the composition of the data packet.
    bool verbosity;                   ///< Whether to push messages during calibration, etc.
    PGASettings pga_settings;         ///< Manages the programmable gain amplifier settings
    PGASettings pga_settings_copy;    ///< Copy of the settings, used to infer which channels to recalibrate
    // TODO: add ADC averaging, resolution to settings?

    /**
     * @brief constructor for the Settings class.
     * 
     * @param sampling_frequency_hz can take any float value (0, 1000]
     * @param game_mode if true, the channel transformation to cartesian coordinates takes place on the Teensy
     * @param verbosity determines whether unsolicited debugging messages will be sent to the user.
     */
    Settings(float sampling_frequency_hz, bool game_mode, bool verbosity);
    /**
     * @brief sets the sampling frequency, in hertz.
     * 
     * @param sampling_frequency_hz can take any float value (0, 1000]
     */
    int setSamplingFrequency(float sampling_frequency_hz);
    /**
     * @brief get the current sampling frequency, in hertz.
     * 
     * @return 1 on failure, 0 on success.
     */
    float getSamplingFrequency();
    /**
     * @brief sets the game mode.
     * 
     * @param game_mode if true, the channel transformation to cartesian coordinates takes place on the Teensy
     */
    int setGameMode(bool game_mode);
    /**
     * @brief get whether game mode is on or not.
     * 
     * @return 0
     */
    bool getGameMode();
    /**
     * @brief sets the verbosity level.
     * 
     * @param verbosity determines whether unsolicited debugging messages will be sent to the user.
     */
    int setVerbosity(bool verbosity);
    /**
     * @brief get the current verbosity level.
     * 
     * @return 0
     */
    bool getVerbosity();
    /**
     * @brief change one or more of the gain settings.
     * 
     * @param finger takes values from [-1, 4]. -1 applies the value to all fingers
     * @param channel takes values from [-1, 4]. -1 applies the value to all channels
     * @param slot takes values from [0, 3]. 0 is the front gain, 1 is the fine gain, 2 is the output gain, 3 is the product.
     * @param value is the proposed value for the slot
     * 
     * @return 1 on failure, 0 on success. Failure can either be due to indexing or violation of a boundary.
     * 
     * @note I really recommend *not* setting individual channels (and probably not individual fingers,
     *       though it's easier to imagine situations where that would be sensible).
     * @note Calls should tend to be settings.setGain(-1, -1, 3, 82.0) or the like.
     * 
     * @todo In verbose mode, should we shout if someone tries to set the individual channel gain?
     */
    int setGain(int8_t finger, int8_t channel, int8_t slot, float value);
    /**
     * @brief get the gain setting for a particular finger, channel, and slot.
     * 
     * @return current gain setting.
     * 
     * @note Usage would be something like gain = settings.getGain(2, 0, 3) for the middle finger, first channel, and product of gains.
     */
    float getGain(int8_t finger, int8_t channel, int8_t slot);
};

#endif
