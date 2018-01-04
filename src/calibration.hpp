#ifndef CALIB_H
#define CALIB_H

namespace calibration {
    int calibrateChannel(unsigned char channel);
    int calibrateAllChannels();
    float inverseTransferFunction(float val, int finger, int channel);
};
#endif
