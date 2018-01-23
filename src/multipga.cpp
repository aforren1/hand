#include "constants.hpp"
#include "multipga.hpp"
#include "i2c_t3.h"
#include "packing.hpp"

namespace cplex = constants::multiplex;

MultiPGA::MultiPGA(PGASettings &pga_settings)
{
    Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
    Wire2.setDefaultTimeout(200000);
    // TODO: not sure if the following loop *does* anything -- based on commented-out code, nothing seems to be set?
    // originally from pga309.cpp
    for (uint8_t chan = 0; chan < constants::pin::n_channels; chan++)
    {
        MultiPGA::setChannel(chan);
    }
    // originally from main.cpp
    MultiPGA::reset();
    MultiPGA::setChannel(0);
}

void MultiPGA::reset()
{
    plex_device = cplex::plex_a_addr;
    plex_channel = 0x01;
}

void MultiPGA::channelEnable()
{
    uint8_t reg_state = 0xFF;
    Wire2.beginTransmission(plex_device);
    Wire2.write(plex_channel);
    Wire2.endTransmission();

    Wire2.requestFrom(plex_device, 1, I2C_NOSTOP);
    while (Wire2.available())
    {
        reg_state = Wire2.readByte();
    }

    if (reg_state == plex_channel)
    {
        // TODO: send message?
    }
}

void MultiPGA::clear()
{
    plex_device = cplex::plex_c_addr;
    uint8_t tmp_plex_channel = plex_channel;
    plex_channel = 0x00;
    MultiPGA::channelEnable();
    plex_device = cplex::plex_b_addr;
    MultiPGA::channelEnable();
    plex_device = cplex::plex_a_addr;
    MultiPGA::channelEnable();
    plex_channel = tmp_plex_channel;
}

void MultiPGA::setChannel(uint8_t chan)
{
    MultiPGA::clear();
    int16_t pga_channel = chan % 8;
    int16_t plex_channel = chan / 8;
    plex_channel = 0x01 << pga_channel;
    plex_device = cplex::plex_a_addr + plex_channel;
    MultiPGA::channelEnable();
}

uint8_t MultiPGA::getDevice()
{
    return plex_device;
}

uint8_t MultiPGA::getChannel()
{
    return plex_channel;
}

// start PGA methods
void MultiPGA::accessRegister(uint8_t addr, bool is_read)
{
    if (addr > 0x08)
    {
        addr = 0x08;
    }
    // TODO: add messages back
    if (is_read)
    {
        MultiPGA::readPGA(addr);
    }
}

void MultiPGA::readPGA(uint8_t addr)
{
    uint8_t n_bytes = 0;
    uint8_t redundancy_bytes = 5;
    uint8_t byte2 = 0x00;
    uint8_t byte1 = 0x00;
    std::array<uint8_t, 2> temp_byte_holder;
    for (uint8_t i = 0; i < redundancy_bytes; i++)
    {
        Wire2.beginTransmission(cplex::pga_addr);
        Wire2.write(addr);
        Wire2.requestFrom(cplex::pga_addr, 2, I2C_NOSTOP);
        n_bytes = Wire2.available();
        while (Wire2.available())
        {
            temp_byte_holder[0] = Wire2.readByte();
            temp_byte_holder[1] = Wire2.readByte();
        }
        Wire2.endTransmission();
    }
    if (addr == 0x00)
    {
        uint16_t count = packing::bigendbytes2num<uint16_t>(temp_byte_holder); //TODO: sanity check?
        float temperature = 0.0625 * count;
        // send temperature message
    }
    else
    {
        uint16_t error_code = packing::bigendbytes2num<uint16_t>(temp_byte_holder);
        // send error code somewhere
    }
}

/**
*   @brief  Computes setting hex needed to represent intended fine offset
*
*   @param fine_offset Desired fine offset as a fraction of the reference voltage
*   @return Resulting 16-bit hex value for setting and value
**/
uint16_t fineOffset2Hex(float fine_offset)
{
    return fine_offset * constants::adc::max_int;
}

uint16_t fineGain2Hex(float fine_gain)
{
    return (fine_gain - 0.33333333) * 1.5 * constants::adc::max_int;
}

// TODO: Implement refCtrlLinear from Jacob's code

uint16_t gainsAndOffset2Hex(float des_front_gain, float des_output_gain, float des_coarse_offset)
{
    uint8_t owd = 0x01;
    uint8_t gi3 = 0x00;
    uint8_t rfb = 0x00;
    uint8_t os = abs(des_coarse_offset) / (constants::calibration::vcc * 0.85);
    uint8_t os5 = 0x00;
    if (des_coarse_offset < 0)
    {
        os5 = 0x01;
    }
    // Limits OS register for magnitude, prevents overflow into negative offset and other registers
    if (os > 0x0F)
    {
        os = 0x0F;
    }
    uint8_t output_gain = mapDesiredOutToReal(des_output_gain);
    uint8_t front_gain = mapDesiredFrontToReal(des_front_gain);

    uint16_t result = owd; result = result << 3;
    result |= output_gain; result = result << 1;
    result |= gi3; result = result << 3;
    result |= front_gain; result = result << 3;
    result |= rfb; result = result << 1;
    result |= os5; result = result << 4;
    result |= os;
    return result;
}

// TODO: Implement PGAConfigLimit from Jacob's code
// TODO: implement TempADCCtrl from Jacob's code
// TODO: implement OutEnableCounterCtrl from Jacob's code
// NOTE: tempConvert has been replaced

uint8_t mapDesiredFrontToReal(float des_front_gain)
{
  uint8_t front_gain;
  if (des_front_gain >= 128) {
    front_gain = 0x07;
  } else if (des_front_gain >= 63.5) {
    front_gain = 0x06;
  } else if (des_front_gain >= 42) {
    front_gain = 0x05;
  } else if (des_front_gain >= 31.5) {
    front_gain = 0x04;
  } else if (des_front_gain >= 23.5) {
    front_gain = 0x03;
  } else if (des_front_gain >= 15.5) {
    front_gain = 0x02;
  } else if (des_front_gain >= 7.5) {
    front_gain = 0x01;
  } else {
    front_gain = 0x00; // Gain is 4
  }
  return front_gain;
}

uint8_t mapDesiredOutToReal(float des_output_gain)
{
    uint8_t output_gain;
    if (des_output_gain >= 8.5)
    {
        output_gain = 0x06; // gain of 9
    }
    else if (output_gain >= 5.5)
    {
        output_gain = 0x05; // gain of 6
    }
    else if (output_gain >= 4)
    {
        output_gain = 0x04; // gain of 4.5
    }
    else if (output_gain >= 3.1)
    {
        output_gain = 0x03; // gain of 3.6
    }
    else if (output_gain >= 2.5)
    {
        output_gain = 0x02; // gain of 3
    }
    else if (output_gain >= 2.1)
    {
        output_gain = 0x01; // gain of 2.4
    }
    else
    {
        output_gain = 0x00; // gain of 2
    }
    return output_gain;
}