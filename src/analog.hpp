#ifndef ANALOG_H
#define ANALOG_H
#include <array>

namespace analog
{
/**
 * @brief Sets the ADCs to the default settings.
 * 
 * @return void
 */
void setupADC();

/**
 * @brief Read all analog pins once.
 * 
 * @param recent_values is a reference to a std::array of type uint16_t, with length matching the number of analog pins to read (in our case, always 20). 
 *        The results of the call to analogRead are stored here.
 * @return void
 */
void readAllOnce(std::array<uint16_t, 20> &recent_values);

/**
 * @brief Read the state of a single analog pin, and convert to millivolts. Primarily used during the calibration procedure.
 * 
 * @param The pin to read (e.g. A1, A2...)
 * @return float value of the analog reading in millivolts
 */
float readChannelMillivolt(uint8_t channel);

/**
 * @brief Convert raw channel readings to (x, y, z).
 * 
 * @param recent_values is a reference to a std::array of type uint16_t (usually the same one used by analog::readAllOnce) and length 20.
 * @param converted_values is a reference to a std::array of type float and length 15. The results of the function are stored here.
 * @return void
 */
void calcRotation(const std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values);

void calcError(const std::array<uint16_t, 20> &recent_values, std::array<float, 5> &converted_values);
};

#endif
