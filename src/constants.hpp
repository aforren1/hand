#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>

namespace constants {
    namespace pin {
        const std::array<unsigned int, 20> sensor_pins = {A12, A13, A16, A14, A17, 
                                                          A18, A19, A20, A21, A22, 
                                                          A0, A1, A5, A4, A3, 
                                                          A2, A6, A7, A8, A9};
        const unsigned int led_for_calibration = 0;
        const unsigned int led_for_adc = 1;
    };

    namespace adc {
        const unsigned int resolution = 16; /// ADC resolution (TODO: allow user to change)
        const unsigned int averaging = 16; /// Number of averaged samples *within* ADC
    };

    struct calibration {
        const unsigned int settling_ms = 100; /// settling time during calibration
        const unsigned int max_iter = 1; /// maximum iterations in optimization?
        const unsigned int n_adc_readings = 1000; /// number of ADC readings to average over
        const float target_fraction = 0.5; /// Trying to land between 0 and 1?
    };
};

#endif
