#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>
#include <array>
#include "ADC.h"

namespace constants
{
namespace pin
{
const uint8_t n_channels = 20;
const std::array<uint8_t, 20> sensor_pins = {A12, A13, A16, A14, A17,
                                                  A18, A19, A20, A21, A22,
                                                  A0, A1, A5, A4, A3,
                                                  A2, A6, A7, A8, A9};
const std::array<std::array<uint8_t, 4>, 5> sensor_pins_nested = {{{A12, A13, A16, A14},
                                                                        {A17, A18, A19, A20},
                                                                        {A21, A22, A0, A1},
                                                                        {A5, A4, A3, A2},
                                                                        {A6, A7, A8, A9}}};
const unsigned int led_for_calibration = 0;
const unsigned int led_for_adc = 1;
};

namespace adc
{
const float pi = 4.0 * atan(1.0);
const unsigned int resolution = 16; ///< ADC resolution (TODO: allow user to change)
const unsigned int averaging = 16;  ///< Number of averaged samples *within* ADC
const float cos_rot = cos(pi / 4.0);
const float sin_rot = sin(pi / 4.0);
const float max_int = 65535;
};

namespace calibration
{
const unsigned int settling_ms = 100;     ///< settling time during calibration
const unsigned int max_iter = 1;          ///< maximum iterations in optimization?
const unsigned int max_fine_iter = 10;    ///< Maximum iterations of fine step
const unsigned int n_adc_readings = 1000; ///< number of ADC readings to average over
const float target_fraction = 0.5;        ///< Trying to land between 0 and 1?
const float vcc = 3.33;                   ///< Matching Jacob here, but should it be 3.3?
const float mvcc = vcc * 1000;
const float tol_mv = 20; ///< Used for fine adjustments
};

namespace multiplex
{
const uint8_t pga_addr = 0x40;
const uint8_t plex_a_addr = 0x70;
const uint8_t plex_b_addr = 0x71;
const uint8_t plex_c_addr = 0x72;
};
};

#endif
