#ifndef MULTIPGA_H
#define MULTIPGA_H

#include "settings.hpp"

struct MultiPGA
{
    uint8_t plex_channel;
    uint8_t plex_device;

    MultiPGA(PGASettings &pga_settings); // init method (start comm, feed in default settings)
    void reset();                        // set device and channel to default (== plexReset)
    uint8_t getDevice();                 // get the plex device the I2C master is on (== getPlexDevice)
    uint8_t getChannel();                // get plex channel the I2C master is on (== getPlexChan)
    void setChannel(uint8_t channel);    // select single I2C channel across all multiplexers (== plexSelect)
    void clear();                        // clear all I2C channels across all multiplexers (== plexClear)
    void channelEnable();                // sends channel enable message to multiplexer with target address (== switchPlex)
    void plexTest();                     // haven't written yet (see old code)

    // originally from pga309.cpp
    void accessRegister(uint8_t addr, bool is_read);
    void readPGA(uint8_t addr);
    uint16_t writePGA(uint8_t addr, float val1, float val2, float val3);
};

// helper functions
uint16_t fineOffset2Hex(float fine_offset);
uint16_t fineGain2Hex(float fine_gain);
uint16_t gainsAndOffset2Hex(float des_fine_gain, float des_output_gain, float des_coarse_offset);
uint8_t mapDesiredFrontToReal(float des_front_gain);
uint8_t mapDesiredOutToReal(float des_output_gain);
uint16_t writeSelect(uint8_t addr, float val1, float val2, float val3);
#endif
