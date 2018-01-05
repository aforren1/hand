#ifndef MULTI_H
#define MULTI_H

#include "constants.hpp"

// device is the tca9548a

struct Multiplexer {
    uint8_t current_plex;
    uint8_t target_plex;

    void reset();
    uint8_t getDevice();
    uint8_t getChannel();
    void selectChannel(uint8_t channel);
    void clear();
    void sendEnable(uint8_t ctrl_reg);
    void test(); // Test routine for accessing all multiplexers
};

#endif
