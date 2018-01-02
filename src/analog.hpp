#include <ADC.h>
#include "constants.hpp"

ADC *adc = new ADC();

elapsedMillis timestamp;
elapsedMicros between_readings_timer;

void setupADC() {
    adc->setResolution(constants::adc::resolution);
    adc->setAveraging(constants::adc::averaging);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
    adc->setReference(ADC_REFERENCE::REF_EXT);
    timestamp = 0;
    between_readings_timer = 0;
}

void readSingle(std::array<uint8_t, 20> &recent_values) {
    for (std::size_t i = 0; i < constants::pin::sensor_pins.size(); ++i) {
        recent_values[i] = adc->analogRead(constants::pin::sensor_pins[i]);
    }
}
