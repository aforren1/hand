#include "Arduino.h"
#include <array>
#include <string>
#include "serial_comm.hpp"

void communication::initComm()
{
    Serial.begin(9600);
    Serial.setTimeout(0);
}

void communication::sendSample(const std::array<float, 15> &game_sample)
{
    for (const auto &i : game_sample)
    {
        Serial.print(i);
        Serial.print(' ');
    }
    Serial.print('\n');
    Serial.send_now();
}

void communication::sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample)
{
    Serial.print(timestamp);
    Serial.print(' ');
    Serial.print(deviation);
    Serial.print(' ');
    for (const auto &i : raw_sample)
    {
        Serial.print(i);
        Serial.print(' ');
    }
    Serial.print('\n');
    Serial.send_now();
}

int communication::receiveRawPacket(std::array<uint8_t, 64> &rx_data)
{
    int n = 0;
    n = Serial.readBytes((char *)rx_data.data(), 64); // TODO: might be bug in Teensy code?
    if (n < 1)
    {
        return 0; // not enough data received--we want the full packet
    }
    return n;
}

void communication::sendRawPacket(std::array<uint8_t, 64> &tx_data)
{
    Serial.write(tx_data.data(), 64);
}

void communication::sendString(std::string tx_string)
{
    Serial.write(tx_string.append(64 - tx_string.length(), ' ').c_str(), 64);
}

void communication::sendSample(const std::array<float, 5> &err_sample)
{
    for (const auto &i : err_sample)
    {
        Serial.print(i);
        Serial.print(' ');
    }
    Serial.print('\n');
    Serial.send_now();
}
