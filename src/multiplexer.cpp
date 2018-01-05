#include "i2c_t3.h"
#include "multiplexer.hpp"
#include "constants.hpp"

namespace cplex = constants::multiplex;

void Multiplexer::reset()
{
    target_plex = cplex::plex_a_addr;
    current_plex = 0x01;
}

uint8_t Multiplexer::getDevice()
{
    return target_plex;
}

uint8_t Multiplexer::getChannel() {
    return current_plex;
}

void Multiplexer::selectChannel(uint8_t channel)
{
    Multiplexer::clear();
    int pga_channel = channel % 8;
    int plex_channel = channel / 8;
    current_plex = 0x01 << pga_channel;
    target_plex = cplex::plex_a_addr + plex_channel;
    Multiplexer::sendEnable(current_plex);
}

void Multiplexer::sendEnable(uint8_t ctrl_reg)
{
    uint8_t reg_state = 0xFF;
    Wire2.beginTransmission(target_plex);
    Wire2.write(ctrl_reg);
    Wire2.endTransmission();
    
    Wire2.requestFrom(target_plex, 1, I2C_NOSTOP);
    while (Wire2.available())
    {
        reg_state = Wire2.readByte();
    }
    if (reg_state == ctrl_reg)
    {
        // notification?
    }
    // other checks
}

void Multiplexer::clear()
{
    target_plex = cplex::plex_c_addr;
    Multiplexer::sendMessage(0x00);
    target_plex = cplex::plex_b_addr;
    Multiplexer::sendMessage(0x00);
    target_plex = cplex::plex_a_addr;
    Multiplexer::sendMessage(0x00);
}