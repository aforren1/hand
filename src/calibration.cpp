#include "constants.hpp"
#include "analog.hpp"

namespace cpin = constants::pin;
namespace ccalib = constants::calibration;

int runCalibration()
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