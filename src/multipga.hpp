#ifndef MULTIPGA_H
#define MULTIPGA_H

#include "settings.hpp"

struct MultiPGA {
    uint8_t plex_channel;
    uint8_t plex_device;

    MultiPGA(PGASettings& pga_settings); // init method (start comm, feed in default settings)
    void reset(); // set device and channel to default (== plexReset)
    uint8_t getDevice(); // get the plex device the I2C master is on (== getPlexDevice)
    uint8_t getChannel(); // get plex channel the I2C master is on (== getPlexChan)
    void setChannel(uint8_t channel); // select single I2C channel across all multiplexers (== plexSelect)
    void clear(); // clear all I2C channels across all multiplexers (== plexClear)
    void channelEnable(); // sends channel enable message to multiplexer with target address (== switchPlex)
    void plexTest(); // haven't written yet (see old code)

    // originally from pga309.cpp
    void accessRegister(uint8_t addr, bool is_read);
    void readPGA(uint8_t addr);

};


#endif
