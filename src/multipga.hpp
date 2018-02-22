#ifndef MULTIPGA_H
#define MULTIPGA_H

#include "settings.hpp"

namespace multipga
{
void init();                                     // init method (start comm, feed in default settings)
void setChannel(uint8_t channel);                // select single I2C channel across all multiplexers (== plexSelect)
void clear();                                    // clear all I2C channels across all multiplexers (== plexClear)
void enableChannel(uint8_t device, uint8_t msg); // sends channel enable message to multiplexer with target address (== switchPlex)

// originally from pga309.cpp
void readPGA(uint8_t addr);
uint16_t writePGA(uint8_t addr, float val1, float val2, float val3);
}

// helper functions
/**
*   @brief  Computes setting hex needed to represent intended fine offset
*
*   @param fine_offset Desired fine offset as a fraction of the reference voltage
*   @return Resulting 16-bit hex value for setting and value
**/
uint16_t fineOffsetToHex(float fine_offset);
/**
*   @brief  Computes setting hex needed to represent intended fine gain
*
*   @param fine_gain Desired fine gain
*   @return Resulting 16-bit hex value for setting and value
**/
uint16_t fineGainToHex(float fine_gain);
/**
*   @brief  Computes setting hex needed to represent intended front-end and output gains, and coarse offset
*
*   @param des_front_gain Desired front-end gain
*   @param des_output_gain Desired output gain
*   @param des_coarse_offset Desired coarse offset
*   @return Resulting 16-bit hex value for setting and value
*   @todo Tables have been omitted for brevity, but we can link to the source?
**/
uint16_t gainsAndOffset2Hex(float des_fine_gain, float des_output_gain, float des_coarse_offset);
uint8_t mapDesiredFrontToReal(float des_front_gain);
uint8_t mapDesiredOutToReal(float des_output_gain);
uint16_t writeSelect(uint8_t addr, float val1, float val2, float val3);
#endif
