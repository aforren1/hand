#ifndef MULTIPGA_H
#define MULTIPGA_H

#include "settings.hpp"

namespace multipga
{
void init();                                     // init method (start comm, feed in default settings)
void setChannel(uint8_t channel);                // select single I2C channel across all multiplexers (== plexSelect)
void clear();                                    // clear all I2C channels across all multiplexers (== plexClear)
void enableChannel(uint8_t device, uint8_t msg); // sends channel enable message to multiplexer with target address (== switchPlex)
void plexTest();                                 // haven't written yet (see old code)

// originally from pga309.cpp
void readPGA(uint8_t addr);
uint16_t writePGA(uint8_t addr, float val1, float val2, float val3);
}

// helper functions
uint16_t fineOffsetToHex(float fine_offset);
uint16_t fineGainToHex(float fine_gain);
uint16_t gainsAndOffset2Hex(float des_fine_gain, float des_output_gain, float des_coarse_offset);
uint8_t mapDesiredFrontToReal(float des_front_gain);
uint8_t mapDesiredOutToReal(float des_output_gain);
uint16_t writeSelect(uint8_t addr, float val1, float val2, float val3);
#endif
