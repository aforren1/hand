#ifndef CALIB_H
#define CALIB_H

#include "settings.hpp"

namespace calibration {
    int calibrateChannel(uint8_t channel, std::array<float, 6> &gain_vec);
    int calibrateAllChannels(PGASettings &pga_settings);
    float inverseTransferFunction(float val, std::array<float, 6> &gain_vec);
};
#endif
