#include <ADC.h>
#include "constants.hpp"
#include "analog.hpp"

namespace cadc = constants::adc;
namespace cpin = constants::pin;

ADC *adc = new ADC();

void analog::setupADC()
{
    adc->setResolution(cadc::resolution);
    adc->setAveraging(cadc::averaging);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
    adc->setReference(ADC_REFERENCE::REF_EXT);
}

void analog::readSingle(std::array<uint16_t, 20> &recent_values) {
    for (std::size_t i = 0; i < cpin::sensor_pins.size(); ++i) {
        recent_values[i] = adc->analogRead(cpin::sensor_pins[i]);
    }
}

void analog::applyRotation(std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values) {
    // scale to [0, 1] and calculate x, y, z forces. (think about this hard, though...)
    int j = 0;
    for (int i = 0; i < 15; i += 3) {
        converted_values[i] = (recent_values[j]/cadc::max_int) * cadc::cos_rot - // x
                                (recent_values[j+1]/cadc::max_int) * cadc::sin_rot;
        converted_values[i+1] = (recent_values[j]/cadc::max_int) * cadc::sin_rot - // y
                                (recent_values[j+1]/cadc::max_int) * cadc::cos_rot;
        converted_values[i+2] = (recent_values[j+2]/cadc::max_int) + (recent_values[j+3]/cadc::max_int); // TODO: might be subtract??
    }
}

