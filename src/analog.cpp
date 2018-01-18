#include "ADC.h"
#include "constants.hpp"
#include "analog.hpp"

namespace cadc = constants::adc;
namespace cpin = constants::pin;

ADC *adc = new ADC();

/**
 * @brief Sets the ADCs to the default settings.
 * 
 * @return void 
 * 
 */
void analog::setupADC()
{
    adc->setResolution(cadc::resolution);
    adc->setAveraging(cadc::averaging);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
    adc->setReference(ADC_REFERENCE::REF_EXT);
}

/**
 * @brief Read all analog pins once.
 * 
 * @param recent_values is a reference to a std::array of type uint16_t, with length matching the number of analog pins to read (in our case, always 20). 
 *        The results of the call to analogRead are stored here.
 */
void analog::readAllOnce(std::array<uint16_t, 20> &recent_values)
{
    for (std::size_t i = 0; i < cpin::sensor_pins.size(); ++i)
    {
        recent_values[i] = adc->analogRead(cpin::sensor_pins[i]);
    }
}

/**
 * @brief Read the state of a single analog pin, and convert to millivolts. Primarily used during the calibration procedure.
 * 
 * @param The pin to read (e.g. A1, A2...)
 */
float readChannelMillivolt(uint8_t channel)
{
    // returns analog reading in millivolts
    unsigned int val = adc->analogRead(channel);
    return val * 3.3 / adc->getMaxValue(ADC_0) * 1000;
}

/**
 * @brief Convert raw channel readings to (x, y, z).
 * 
 * @param recent_values is a reference to a std::array of type uint16_t (usually the same one used by analog::readAllOnce) and length 20.
 * @param converted_values is a reference to a std::array of type float and length 15. The results of the function are stored here.
 */
void analog::applyRotation(std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values)
{
    // scale to [0, 1] and calculate x, y, z forces. (think about this hard, though...)
    int j = 0;
    for (int i = 0; i < 15; i += 3)
    {
        converted_values[i] = (recent_values[j] / cadc::max_int) * cadc::cos_rot - // x
                              (recent_values[j + 1] / cadc::max_int) * cadc::sin_rot;
        converted_values[i + 1] = (recent_values[j] / cadc::max_int) * cadc::sin_rot - // y
                                  (recent_values[j + 1] / cadc::max_int) * cadc::cos_rot;
        converted_values[i + 2] = (recent_values[j + 2] / cadc::max_int) + (recent_values[j + 3] / cadc::max_int); // TODO: might be subtract??
        j += 4;
    }
}
