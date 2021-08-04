#include "ADC.h"
#include "constants.hpp"
#include "analog.hpp"

namespace cadc = constants::adc;
namespace cpin = constants::pin;

ADC *adc = new ADC();

void analog::setupADC()
{
    for (ADC_Module *const module : {adc->adc0,adc->adc1}){
        module->setResolution(cadc::resolution);
        module->setAveraging(cadc::averaging);
        module->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
        module->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
        module->setReference(ADC_REFERENCE::REF_EXT);    
    }
}

void analog::readAllOnce(std::array<uint16_t, 20> &recent_values)
{
    for (std::size_t i = 0; i < cpin::sensor_pins.size(); ++i)
    {
        recent_values[i] = adc->analogRead(cpin::sensor_pins[i]);
    }
}

float analog::readChannelMillivolt(uint8_t channel)
{
    // returns analog reading in millivolts
    unsigned int val = adc->analogRead(channel);
    return val * 3.3 / adc->adc0->getMaxValue() * 1000;
}

void analog::calcRotation(const std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values)
{
    int j = 0;
    for (int i = 0; i < 15; i += 3)
    {
        converted_values[i] = ((recent_values[j] / cadc::max_int) - (recent_values[j + 1] / cadc::max_int)) / cadc::sqrt2;
        converted_values[i + 1] = ((recent_values[j] / cadc::max_int) + (recent_values[j + 1] / cadc::max_int)) / cadc::sqrt2;
        converted_values[i + 2] = (recent_values[j + 2] / cadc::max_int) + (recent_values[j + 3] / cadc::max_int);
        j += 4;
    }
}

void analog::calcError(const std::array<uint16_t, 20> &recent_values, std::array<float, 5> &converted_values)
{
    int j = 0;
    for (int i = 0; i < 5; i += 3)
    {
        converted_values[i] = abs(recent_values[j] / cadc::max_int -
                                  recent_values[j + 1] / cadc::max_int +
                                  recent_values[j + 2] / cadc::max_int -
                                  recent_values[j + 3] / cadc::max_int);
        j += 4;
    }
}
