#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>
#include <array>
#include <climits>
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
const std::array<std::array<uint8_t, 4>, 5> mux_pins_nested = {{{0, 1, 2, 3},
                                                                {4, 5, 6, 7},
                                                                {8, 9, 10, 11},
                                                                {12, 13, 14, 15},
                                                                {16, 17, 18, 19}}};
const unsigned int led_for_calibration = 0;
const unsigned int led_for_adc = 1;
};

namespace adc
{
const unsigned int resolution = 16; ///< ADC resolution (TODO: allow user to change)
const unsigned int averaging = 16;  ///< Number of averaged samples *within* ADC
const float max_int = USHRT_MAX;
const float sqrt2 = sqrt(2);
};

namespace calibration
{
const unsigned int settling_ms = 100;     ///< settling time during calibration
const uint16_t max_iter = 1;              ///< maximum iterations in optimization?
const uint16_t max_fine_iter = 10;        ///< Maximum iterations of fine step
const unsigned int n_adc_readings = 1000; ///< number of ADC readings to average over
const float target_fraction = 0.5;        ///< Trying to land between 0 and 1?
const float vcc = 3.3;                   ///< 3.3 V reference from https://datasheet.octopart.com/ADP150AUJZ-3.3-R7-Analog-Devices-datasheet-8464465.pdf
const float mvcc = vcc * 1000;
const float tol_mv = 20; ///< Used for fine adjustments
const float coarse_max = 14 * 0.85 * vcc; // Vcc = mVcc * 10^-3, coarse max units in mV
};

namespace multiplex
{
const uint8_t pga_addr = 0x40;
const uint8_t plex_a_addr = 0x70;
const uint8_t plex_b_addr = 0x71;
const uint8_t plex_c_addr = 0x72;
enum PLXMDL {p9847, p9848};
const PLXMDL plxUsed = p9848; // Switch to p9848 if a simultaneous channel enable plex is used, switch to p9847 if a single channel enable plex is used

const std::array<std::string, 13> i2c_names = {{"I2C_WAITING",   // stopped states
                                                "I2C_TIMEOUT",   //  |
                                                "I2C_ADDR_NAK",  //  |
                                                "I2C_DATA_NAK",  //  |
                                                "I2C_ARB_LOST",  //  |
                                                "I2C_BUF_OVF",   //  |
                                                "I2C_NOT_ACQ",   //  |
                                                "I2C_DMA_ERR",   //  V
                                                "I2C_SENDING",   // active states
                                                "I2C_SEND_ADDR", //  |
                                                "I2C_RECEIVING", //  |
                                                "I2C_SLAVE_TX",  //  |
                                                "I2C_SLAVE_RX"}};

const std::array<float, 6> power_on_settings = {{4.0, 0.5, 2.0, 4.0, 0.0, 0.25*constants::calibration::mvcc}};

};
};

#endif
