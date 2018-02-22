#include <array>
#include <numeric>
#include "i2c_t3.h"
#include "constants.hpp"
#include "analog.hpp"
#include "calibration.hpp"
#include "settings.hpp"
#include "multipga.hpp"
#include "comm.hpp"

namespace cpin = constants::pin;
namespace ccalib = constants::calibration;
namespace cplex = constants::multiplex;

int calibration::calibrateAllChannels(PGASettings &pga_settings)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            calibration::calibrateChannel(cpin::mux_pins_nested[i][j], pga_settings.gains_and_offsets[i][j]);
        }
    }
    return 0;
}

int calibration::calibrateChannel(uint8_t mux_channel, std::array<float, 6> &gain_vec)
// 0th index is front gain, 1st index is fine gain, 2nd index is output gain, 3rd is product (not used here)
{
    digitalWriteFast(constants::pin::led_for_calibration, HIGH);
    comm::sendString("Starting calibration on channel " + std::to_string(mux_channel));
    comm::sendString("Analog pin: " + std::to_string(cpin::sensor_pins[mux_channel]));
    comm::sendString("Desired settings: ");
    comm::sendString("Frontend gain: " + std::to_string(gain_vec[0]));
    comm::sendString("Fine gain: " + std::to_string(gain_vec[1]));
    comm::sendString("Output gain: " + std::to_string(gain_vec[2]));
    comm::sendString("Product of gains: " + std::to_string(gain_vec[3]));
    comm::sendString("Coarse offset: " + std::to_string(gain_vec[4]));
    comm::sendString("Fine offset: " + std::to_string(gain_vec[5]));

    float desired_front_gain = gain_vec[0];
    float desired_output_gain = gain_vec[1];
    float desired_fine_gain = gain_vec[2];
    uint16_t iter;
    std::array<float, ccalib::n_adc_readings> adc_readings;
    // coarse adjustments first
    for (iter = 0; iter < ccalib::max_iter; iter++)
    {
        for (std::size_t i = 0; i < adc_readings.size(); ++i)
        {
            adc_readings[i] = analog::readChannelMillivolt(cpin::sensor_pins[mux_channel]);
        }
        float sum = std::accumulate(adc_readings.begin(), adc_readings.end(), 0);
        float average_mv = sum / ccalib::n_adc_readings;
        float diff_mv = calibration::inverseTransferFunction(average_mv, gain_vec);
        float adjust_mv = average_mv - (ccalib::mvcc * ccalib::target_fraction);
        float error_mv = diff_mv - 0; // ? why
        float error_out = 0;
        comm::sendString("Average reading: " + std::to_string(average_mv));
        comm::sendString("Result from inverse transfer function: " + std::to_string(diff_mv));
        comm::sendString("Adjustment (?): " + std::to_string(adjust_mv));
        comm::sendString("Error mV (?): " + std::to_string(error_mv));
        if (!iter)
        { // first iteration
            error_out = average_mv - (ccalib::mvcc / 4.0);
        }
        else
        {
            error_out = adjust_mv;
        }
        comm::sendString("Error mV, part 2: " + std::to_string(error_out));
        multipga::setChannel(mux_channel); // enable intended PGA channel

        // PART 1: Modify sensitivity
        float coarse_mv_max = ccalib::vcc * 14 * 0.85; // TODO: make const
        float excess_mv = 0;
        if (error_mv > coarse_mv_max)
        {
            excess_mv = error_mv - coarse_mv_max;
            error_mv = coarse_mv_max;
            comm::sendString("Calibration exceeded coarse (?), value: " + std::to_string(excess_mv));
        }
        else if (error_mv < -coarse_mv_max)
        {
            excess_mv = error_mv + coarse_mv_max;
            error_mv = -coarse_mv_max;
            comm::sendString("Calibration exceeded coarse (?), value: " + std::to_string(excess_mv));
        }

        float desired_coarse_offset = -error_mv;
        uint16_t gain_cmsg = multipga::writePGA(0x04, desired_front_gain, desired_output_gain, desired_coarse_offset);
        uint16_t coarse_multi = gain_cmsg & 0x000f;
        uint16_t coarse_sign = (gain_cmsg & 0x0010) >> 4;
        comm::sendString("Gains and offsets message: " + std::to_string(gain_cmsg));
        comm::sendString("MULTI_MSG (?): " + std::to_string(coarse_multi));
        comm::sendString("SIGN_MSG (?): " + std::to_string(coarse_sign));
        if (coarse_sign == 1)
        {
            excess_mv = error_mv - coarse_multi * ccalib::vcc * 0.85;
            gain_vec[4] = -coarse_multi; // How does this work? I think we store floats normally?
        }
        else
        {
            excess_mv = error_mv + coarse_multi * ccalib::vcc * 0.85;
            gain_vec[4] = coarse_multi;
        }
        comm::sendString("Remaining error: " + std::to_string(excess_mv));

        // PART 2: Modify zero
        float desired_fine_offset = (ccalib::target_fraction / (desired_fine_gain * desired_output_gain)) -
                                    desired_front_gain * excess_mv / (ccalib::mvcc);
        gain_vec[5] = desired_fine_offset;
        uint16_t offset_msg = multipga::writePGA(0x01, desired_fine_offset, 0, 0);
        uint16_t fine_gain_msg = multipga::writePGA(0x02, desired_fine_gain, 0, 0);
        comm::sendString("Fine offset message: " + std::to_string(offset_msg));
        comm::sendString("Fine gain message: " + std::to_string(fine_gain_msg));
        comm::sendString("Post-I2C status: ", cplex::i2c_names[Wire2.status()]);
    }

    // Now we do our fine adjustments
    comm::sendString("Start fine adjustments.");
    for (iter = 0; iter < ccalib::max_fine_iter; iter++)
    {
        float step = 0.0001 / desired_front_gain;
        float multiplier = 3;
        if (desired_front_gain > 30)
        {
            multiplier = 5;
        }
        // ripped from earlier
        for (std::size_t i = 0; i < adc_readings.size(); ++i)
        {
            adc_readings[i] = analog::readChannelMillivolt(cpin::sensor_pins[mux_channel]);
        }
        float sum = std::accumulate(adc_readings.begin(), adc_readings.end(), 0);
        float average_mv = sum / ccalib::n_adc_readings;
        float adjust_mv = average_mv - (ccalib::mvcc * ccalib::target_fraction);
        comm::sendString("Average reading: " + std::to_string(average_mv));
        comm::sendString("Adjustment (?): " + std::to_string(adjust_mv));
        float iter_step = 0;
        if (adjust_mv > ccalib::tol_mv)
        {
            iter_step = -step * multiplier * adjust_mv;
            comm::sendString("Fine offset up by: " + std::to_string(iter_step));
        }
        else if (adjust_mv < -ccalib::tol_mv)
        {
            iter_step = -step * multiplier * adjust_mv;
            comm::sendString("Fine offset down by: " + std::to_string(iter_step));
        }
        else
        {
            comm::sendString("Fine calibration within tolerance of " + std::to_string(ccalib::tol_mv));
            continue; // criterion met
        }
        gain_vec[5] += iter_step; // update fine offset
        multipga::setChannel(mux_channel);
        uint16_t offset_msg = multipga::writePGA(0x01, gain_vec[5], 0, 0);
        comm::sendString("Current fine offset: " + std::to_string(gain_vec[5]));
        unsigned long t0 = millis();
        while (millis() - t0 < ccalib::settling_ms) // allow settling (TODO: revisit?)
        {
        }
    }
    digitalWriteFast(constants::pin::led_for_calibration, LOW); // done calibration
    if (iter < ccalib::max_fine_iter)
    {
        comm::sendString("Calibration finished within tolerance.");
        return 0;
    }
    comm::sendString("Calibration did not finish within tolerance.");
    return 1; // fine failed to find best answer
}

float calibration::inverseTransferFunction(float val, std::array<float, 6> &gain_vec)
{
    // (((val / (fine_gain * output_gain)) - fine_offset) / front_gain) - coarse_offset
    return (((val / (gain_vec[1] * gain_vec[2])) - gain_vec[5]) / gain_vec[0]) - gain_vec[4];
}