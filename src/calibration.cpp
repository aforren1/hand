#include "constants.hpp"
#include "analog.hpp"
#include "calibration.hpp"
#include "settings.hpp"
#include <numeric>

namespace cpin = constants::pin;
namespace ccalib = constants::calibration;

int calibration::calibrateAllChannels()
// should num_channels always be 20? If I understand, the
// current code only calibrates one sensor??
{
    for (unsigned int channel = 0; channel < cpin::sensor_pins.size(); channel++)
    {
        for (unsigned int iter = 0; iter < ccalib::max_iter; iter++)
        {
            // millivolts = (analog value * 3.3/adc->getMaxValue(ADC_0)) * 1000
            // take n measurements from the current analog pin and return average in mV

            // run through the inverse of the transfer function

            // adjust_mv = average_mv - (Vcc)
        }
    }
}

int calibration::calibrateChannel(unsigned char channel)
{
    std::array<float, ccalib::n_adc_readings> adc_readings;
    for (size_t i = 0; i < adc_readings.size(); ++i)
    {
        adc_readings[i] = analog::readChannelMillivolt(channel);
    }
    float sum = std::accumulate(adc_readings.begin(), adc_readings.end(), 0);
    float average = sum / ccalib::n_adc_readings;
}

float calibration::inverseTransferFunction(float val, int finger, int channel)
{
    float gi, gd, go, vc, vf;
    return (((val / (gd * go)) - vf) / gi) - vc;
}