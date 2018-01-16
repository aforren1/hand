#include "constants.hpp"
#include "multipga.hpp"
#include "i2c_t3.h"

namespace cplex = constants::multiplex;


MultiPGA::MultiPGA(PGASettings& pga_settings)
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
    for (uint8_t i = 0; i < redundancy_bytes; i++)
    {
        Wire2.beginTransmission(cplex::pga_addr);
        Wire2.write(addr);
        Wire2.requestFrom(cplex::pga_addr, 2, I2C_NOSTOP);
        n_bytes = Wire2.available();
        while (Wire2.available())
        {
            byte2 = Wire2.readByte();
            byte1 = Wire2.readByte();
        }
        Wire2.endTransmission();
    }
    if (addr == 0x00)
    {
        float temperature = <conversion here>;
    }
}
