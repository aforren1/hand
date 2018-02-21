#include <string>
#include "constants.hpp"
#include "multipga.hpp"
#include "i2c_t3.h"
#include "packing.hpp"
#include "comm.hpp"


namespace cplex = constants::multiplex;

void multipga::init()
{
    // Setup for Master mode, pins 3_4, external pullups, 400kHz, 200ms default timeout
    // Teensy sets 0x00 address, but does not matter as master
    // originally from main.cpp
    Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
    Wire2.setDefaultTimeout(200000); // 200 ms
    // originally from pga309.cpp
    //plex_device = cplex::plex_a_addr; // == targetPlex
    //plex_channel = 0x01;              // == currPlex
    multipga::setChannel(0);
}

void multipga::enableChannel(uint8_t device, uint8_t msg) // == switchPlex from tca9548a.cpp
{
    comm::sendString("Device: ", std::to_string(device));
    uint8_t reg_state = 0xFF;
    Wire2.beginTransmission(device);
    Wire2.write(msg);
    Wire2.endTransmission();

    Wire2.requestFrom(device, 1, I2C_NOSTOP);
    while (Wire2.available())
    {
        reg_state = Wire2.readByte();
    }

    if (reg_state == msg)
    {
        // TODO: send message?
    }
    // TODO: let the user know if comm failed & such
}

void multipga::clear() // == plexClear from tca9548a.cpp
// TODO: Do we need to do this?
{
    multipga::enableChannel(cplex::plex_c_addr, 0x00);
    multipga::enableChannel(cplex::plex_b_addr, 0x00);
    multipga::enableChannel(cplex::plex_a_addr, 0x00);
}

void multipga::setChannel(uint8_t chan) // plexSelect from tca9548a.cpp
{
    multipga::clear();
    int16_t pga_channel = chan % 8;
    int16_t xx = chan / 8; // Sorry about the name, not sure yet...
    int8_t plex_channel = 0x01 << pga_channel;
    multipga::enableChannel(cplex::plex_a_addr + xx, plex_channel);
}

// start PGA methods
void multipga::accessRegister(uint8_t addr, bool is_read) // accessRegister in pga309.cpp
{
    // TODO: toggle builtin LED
    if (addr > 0x08)
    {
        addr = 0x08;
    }
    // TODO: add messages back
    if (is_read)
    {
        multipga::readPGA(addr);
    }
    // TODO: extra messages & potential handling of things?
}

uint16_t multipga::writePGA(uint8_t addr, float val1, float val2, float val3) // writeToPGA in pga309.cpp
{
    Wire2.beginTransmission(cplex::pga_addr);
    Wire2.write(addr);
    int16_t raw_2byte = writeSelect(addr, val1, val2, val3);
    uint8_t write_array[2] = {};
    memcpy(&raw_2byte, write_array, 2);
    Wire2.write(write_array, 2);
    Wire2.endTransmission();
    return raw_2byte;
}

void multipga::readPGA(uint8_t addr)
{
    uint8_t n_bytes;
    uint8_t redundancy_bytes = 5;
    std::array<uint8_t, 2> temp_byte_holder;
    for (uint8_t i = 0; i < redundancy_bytes; i++)
    {
        Wire2.beginTransmission(cplex::pga_addr);
        Wire2.write(addr);
        Wire2.requestFrom(cplex::pga_addr, 2, I2C_NOSTOP);
        n_bytes = Wire2.available();
        while (Wire2.available())
        {
            // Note: tried to back in reverse order, so we can use the packing machinery
            temp_byte_holder[0] = Wire2.readByte();
            temp_byte_holder[1] = Wire2.readByte();
        }
        Wire2.endTransmission();
    }
    if (addr == 0x00)
    {
        uint16_t count = packing::bigendbytes2num<uint16_t>(temp_byte_holder); //TODO: sanity check?
        float temperature = 0.0625 * count;
        // TODO: send temperature message
    }
    else
    {
        uint16_t error_code = packing::bigendbytes2num<uint16_t>(temp_byte_holder);
        // TODO: send error code somewhere
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

/**
*   @brief  Computes setting hex needed to represent intended fine gain
*
*   @param fine_gain Desired fine gain
*   @return Resulting 16-bit hex value for setting and value
**/
uint16_t fineGain2Hex(float fine_gain)
{
    return (fine_gain - 0.33333333) * 1.5 * constants::adc::max_int;
}

// TODO: Implement refCtrlLinear from Jacob's code

/**
*   @brief  Computes setting hex needed to represent intended front-end and output gains, and coarse offset
*
*   @param des_front_gain Desired front-end gain
*   @param des_output_gain Desired output gain
*   @param des_coarse_offset Desired coarse offset
*   @return Resulting 16-bit hex value for setting and value
*   @todo Tables have been omitted for brevity, but we can link to the source?
**/
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

    uint16_t result = owd;
    result = result << 3;
    result |= output_gain;
    result = result << 1;
    result |= gi3;
    result = result << 3;
    result |= front_gain;
    result = result << 3;
    result |= rfb;
    result = result << 1;
    result |= os5;
    result = result << 4;
    result |= os;
    return result;
}

// TODO: Implement PGAConfigLimit from Jacob's code
// TODO: implement TempADCCtrl from Jacob's code
// TODO: implement OutEnableCounterCtrl from Jacob's code
// NOTE: tempConvert has been replaced

uint16_t writeSelect(uint8_t addr, float val1, float val2, float val3) // writeSelect in pga309.cpp
{
    uint16_t write_msg = 0x0000;
    switch (addr)
    {
    case 0x01:
        write_msg = fineOffset2Hex(val1);
        break;
    case 0x02:
        write_msg = fineGain2Hex(val1);
        break;
    case 0x03:
        write_msg = 42;
        break; // refCtrlLinear in Jacob's
    case 0x04:
        write_msg = gainsAndOffset2Hex(val1, val2, val3);
        break;
    case 0x05:
        write_msg = 4096;
        break; // PGAConfigLimit in Jacob's
    case 0x06:
        write_msg = 4096;
        break; // TempADCCtrl in Jacob's
    case 0x07:
        write_msg = 1952;
        break; // OutEnableCounterCtrl in Jacob's
    default:
        write_msg = fineOffset2Hex(val1);
        break;
    }
    return write_msg;
}
uint8_t mapDesiredFrontToReal(float des_front_gain)
{
    uint8_t front_gain;
    if (des_front_gain >= 128)
    {
        front_gain = 0x07;
    }
    else if (des_front_gain >= 63.5)
    {
        front_gain = 0x06;
    }
    else if (des_front_gain >= 42)
    {
        front_gain = 0x05;
    }
    else if (des_front_gain >= 31.5)
    {
        front_gain = 0x04;
    }
    else if (des_front_gain >= 23.5)
    {
        front_gain = 0x03;
    }
    else if (des_front_gain >= 15.5)
    {
        front_gain = 0x02;
    }
    else if (des_front_gain >= 7.5)
    {
        front_gain = 0x01;
    }
    else
    {
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
    else if (des_output_gain >= 5.5)
    {
        output_gain = 0x05; // gain of 6
    }
    else if (des_output_gain >= 4)
    {
        output_gain = 0x04; // gain of 4.5
    }
    else if (des_output_gain >= 3.1)
    {
        output_gain = 0x03; // gain of 3.6
    }
    else if (des_output_gain >= 2.5)
    {
        output_gain = 0x02; // gain of 3
    }
    else if (des_output_gain >= 2.1)
    {
        output_gain = 0x01; // gain of 2.4
    }
    else
    {
        output_gain = 0x00; // gain of 2
    }
    return output_gain;
}