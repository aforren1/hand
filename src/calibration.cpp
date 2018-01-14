#include "constants.hpp"
#include "analog.hpp"
#include "calibration.hpp"
#include "settings.hpp"
#include <array>
#include <numeric>

namespace cpin = constants::pin;
namespace ccalib = constants::calibration;

int calibration::calibrateAllChannels(PGASettings &pga_settings)
// should num_channels always be 20? If I understand, the
// current code only calibrates one sensor??
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            calibration::calibrateChannel(cpin::sensor_pins_nested[i][j], pga_settings.gains_and_offsets[i][j]);
        }
    }
}

int calibration::calibrateChannel(uint8_t channel, std::array<float, 6> &gain_vec)
// 0th index is front gain, 1st index is fine gain, 2nd index is output gain, 3rd is product (not used here)
{
    std::array<float, ccalib::n_adc_readings> adc_readings;
    // coarse adjustments first
    for (int iter = 0; iter < ccalib::max_iter; iter++)
    {
        for (std::size_t i = 0; i < adc_readings.size(); ++i)
        {
            adc_readings[i] = analog::readChannelMillivolt(channel);
        }
        float sum = std::accumulate(adc_readings.begin(), adc_readings.end(), 0);
        float average_mv = sum / ccalib::n_adc_readings;
        float diff_mv = calibration::inverseTransferFunction(average_mv, gain_vec);
        float adjust_mv = average_mv - (ccalib::mvcc * ccalib::target_fraction);
        float error_mv = diff_mv - 0; // ??? why
        float error_out = 0;
        if (!iter)
        { // first iteration
            error_out = average_mv - (ccalib::mvcc / 4);
        }
        else
        {
            error_out = adjust_mv;
        }
        // PART 1: Modify sensitivity

        // enable intended PGA channel
        plexSelect(channel); // TODO: unimplemented
        float desired_front_gain = gain_vec[0];
        float desired_output_gain = gain_vec[2];
        float coarse_mv_max = ccalib::vcc * 14 * 0.85; // TODO: make const
        float excess_mv = 0;
        if (error_mv > coarse_mv_max)
        {
            excess_mv = error_mv - coarse_mv_max;
            error_mv = coarse_mv_max;
        }
        else if (error_mv < -coarse_mv_max)
        {
            excess_mv = error_mv + coarse_mv_max;
            error_mv = -coarse_mv_max;
        }

        float desired_coarse_offset = -error_mv;
        // TODO: unimplemented
        uint16_t gain_cmsg = writeToPGA(0x04, desired_front_gain, desired_output_gain, desired_coarse_offset);
        uint16_t coarse_multi = gain_cmsg & 0x000f;
        uint16_t coarse_sign = (gain_cmsg & 0x0010) >> 4;
        if (coarse_sign) // truthy
        {
            excess_mv = error_mv - coarse_multi * ccalib::vcc * 0.85;
            gain_vec[3] = -coarse_multi;
        }
        else
        {
            excess_mv = error_mv + coarse_multi * ccalib::vcc * 0.85;
            gain_vec[3] = coarse_multi;
        }
        // Part 2: Modify zero
        desired_front_gain = gain_vec[1];
        float desired_fine_offset = (ccalib::target_fraction / (desired_fine_gain * desired_output_gain)) -
                                    desired_front_gain * excess_mv / (ccalib::mvcc);
        gain_vec[4] = desired_fine_offset;
        uint16_t offset_msg = writeToPGA(0x01, desired_fine_offset, 0, 0);
        uint16_t gain_msg = writeToPGA(0x02, desired_fine_gain, 0, 0);
    }

    // Now we do our fine adjustments
    for (int iter2 = 0; iter2 < ccalib::max_fine_iter; iter2++)
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
            adc_readings[i] = analog::readChannelMillivolt(channel);
        }
        float sum = std::accumulate(adc_readings.begin(), adc_readings.end(), 0);
        float average_mv = sum / ccalib::n_adc_readings;
        float adjust_mv = average_mv - (ccalib::mvcc * ccalib::target_fraction);
        float iter_step = 0;
        if (adjust_mv > ccalib::tol_mv)
        {
            iter_step = -step * multiplier * adjust_mv;
        }
        else if (adjust_mv < -ccalib::tol_mv)
        {
            iter_step = -step * multiplier * adjust_mv;
        }
        else
        {
            continue; // criterion met
        }
        gain_vec[5] += iter_step; // update fine offset
        plexSelect(channel);
        offset_msg = writeToPGA(0x01, gain_vec[5], 0, 0);
        // put pause here for settling?
    }
    if (iter2 < ccalib::max_fine_iter)
    {
        return 0;
    }
    return 1; // fine failed to find best answer
}

float calibration::inverseTransferFunction(float val, std::array<float, 6> &gain_vec)
{
    // (((val / (fine_gain * output_gain)) - fine_offset) / front_gain) - coarse_offset
    return (((val / (gain_vec[1] * gain_vec[2])) - gain_vec[5]) / gain_vec[0]) - gain_vec[4];
}