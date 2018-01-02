#include "constants.hpp"
#include "adc.hpp"

namespace pc = PinConstants;
namespace cc = CalibrationConstants;

int runCalibration()
// should num_channels always be 20? If I understand, the
// current code only calibrates one sensor??
{
    for (unsigned int channel = 0; channel < pc::channels; channel++) {
        for (unsigned int iter = 0; iter < cc::max_iter; iter++) {
            // millivolts = (analog value * 3.3/adc->getMaxValue(ADC_0)) * 1000
            // take n measurements from the current analog pin and return average in mV

            // run through the inverse of the transfer function

            // adjust_mv = average_mv - (Vcc)
        }
    }
}