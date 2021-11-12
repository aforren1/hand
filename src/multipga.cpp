#include <string>
#include <array>
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
    comm::sendString("Message: ", std::to_string(msg)); // Enable 1 channel (log2(msg) = chan) or disable all (0x00)
    uint8_t reg_state = 0xFF;
    Wire2.beginTransmission(device);
    Wire2.write(msg);
    Wire2.endTransmission();

    comm::sendString("PLEX_SEND_VERIFY");
    Wire2.requestFrom(device, 1, I2C_NOSTOP);
    while (Wire2.available())
    {
        reg_state = Wire2.readByte();
    }

    if (reg_state == msg)
    {
        comm::sendString("PLEX_REG_NEW");
    }
    comm::sendString("Post-I2C status: ", cplex::i2c_names[Wire2.status()]);
    // Note: Didn't send the last SendByteOverHID(MISC_DIVIDER, PLEX_PRINT_ALL) b/c not sure what it does
}

void multipga::clear() // == plexClear from tca9548a.cpp
{
    comm::sendString("Clearing PLEX.");
    multipga::enableChannel(cplex::plex_c_addr, 0x00);
    multipga::enableChannel(cplex::plex_b_addr, 0x00);
    multipga::enableChannel(cplex::plex_a_addr, 0x00);
}

uint8_t multipga::setChannel(uint8_t chan) // plexSelect from tca9548a.cpp
{
    multipga::clear();
    uint8_t pga_channel = chan % 8;
    uint8_t xx = chan / 8; // Sorry about the name, not sure yet...
    uint8_t plex_channel = 0;
    constants::multiplex::PLXMDL plex_model = constants::multiplex::plxUsed;
    if (plex_model == constants::multiplex::p9848) // Model allows for simultaneous 8-bit chan enable
    {
        plex_channel = 0x01 << pga_channel;
    }
    else if (plex_model == constants::multiplex::p9847) // Model uses 3-bits to enable 8 channels + 1-bit disable (B3), so no sim. enable
    {
        plex_channel = 0x08 + pga_channel; // enable bit at B3 + 7-bit target pga
    }
    else
    {
        return -1;
    }
    multipga::enableChannel(cplex::plex_a_addr + xx, plex_channel);
    return 0;
}

uint16_t multipga::writePGA(uint8_t addr, float val1, float val2, float val3) // writeToPGA in pga309.cpp
{
    comm::sendString("Sending vals " + std::to_string(val1) + ", " +
                     std::to_string(val2) + ", " + std::to_string(val3) +
                     " to address " + std::to_string(addr));
    Wire2.beginTransmission(cplex::pga_addr);
    Wire2.write(addr);
    uint16_t raw_2byte = writeSelect(addr, val1, val2, val3);
    uint8_t write_array[2] = {};
    memcpy(write_array, &raw_2byte, 2);
    Wire2.write(write_array, 2);
    // Reports success or failure of write transmit
    switch (Wire2.endTransmission())
    {
    case 0:
        comm::sendString("Successful write to amplifier.");
        break;
    case 1:
        comm::sendString("***ERROR: Write buffer overflow. Please replug device.***");
        break;
    case 2:
        comm::sendString("***ERROR: Amplifier did not acknowledge address. Please replug device.***");
        break;
    case 3:
        comm::sendString("***ERROR: Amplifier did not acknowledge data. Please replug device.***");
        break;
    case 4:
        comm::sendString("***ERROR: Unknown write error to amplifier. Please replug device.***");
        break;  
    default:
        break;
    }
    if (readPGA(addr) != raw_2byte){
        comm::sendString("***ERROR: Stored register value read does not match write message. Replugging is advised.***");
    }
    // TODO: Return error codes instead if NACK or read mismatch occurs.
    // TODO: Repeat write if NACK occurs so unplugging is not required.
    return raw_2byte;
}

uint16_t multipga::readPGA(uint8_t addr)
{
    uint8_t redundancy_bytes = 5;
    std::array<uint8_t, 2> temp_byte_holder;
    for (uint8_t i = 0; i < redundancy_bytes; i++)
    {
        Wire2.beginTransmission(cplex::pga_addr);
        Wire2.write(addr);
        Wire2.requestFrom(cplex::pga_addr, 2, I2C_NOSTOP);
        while (Wire2.available())
        {
            temp_byte_holder[1] = Wire2.readByte();
            temp_byte_holder[0] = Wire2.readByte();
        }
        if (Wire2.endTransmission() != 0) // returns 0 if slave acknowledge successful, non-0 if unsucessful
        {
            comm::sendString("ERROR: I2C amplifier failed to acknowledge.");
            return 0; // Zero message if NACK
        }
    }
    uint16_t pgaMsg = 0;
    if (addr == 0x00)
    {
        uint16_t count = packing::bigEndBytesToNum<uint16_t>(temp_byte_holder); //TODO: sanity check?
        float temperature = 0.0625 * count;
        comm::sendString("Temperature: " + std::to_string(temperature));
        pgaMsg = count;
    }
    else
    {
        uint16_t error_code = packing::bigEndBytesToNum<uint16_t>(temp_byte_holder);
        comm::sendString("Read code: " + std::to_string(error_code));
        pgaMsg = error_code;
    }
    return pgaMsg;
}

uint16_t fineOffsetToHex(float fine_offset)
{
    return fine_offset * constants::adc::max_int;
}

uint16_t fineGainToHex(float fine_gain)
{
    return (fine_gain - 0.33333333) * 1.5 * constants::adc::max_int;
}

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

// TODO: Implement refCtrlLinear from Jacob's code
// TODO: Implement PGAConfigLimit from Jacob's code
// TODO: implement TempADCCtrl from Jacob's code
// TODO: implement OutEnableCounterCtrl from Jacob's code

// NOTE: only 0x01, 0x02, and 0x04 are used
uint16_t writeSelect(uint8_t addr, float val1, float val2, float val3) // writeSelect in pga309.cpp
{
    uint16_t write_msg = 0x0000;
    switch (addr)
    {
    case 0x01:
        write_msg = fineOffsetToHex(val1);
        break;
    case 0x02:
        write_msg = fineGainToHex(val1);
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
        write_msg = fineOffsetToHex(val1);
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
