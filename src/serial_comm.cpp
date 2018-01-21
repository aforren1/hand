#include "Arduino.h"
#include <array>
#include "communication.hpp"

void communication::initComm()
{
    Serial.begin(9600);
    Serial.setTimeout(0); // return immediately (match HID)
}

void communication::sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
    for (auto i : game_sample)
    {
        Serial.print(i);
        Serial.print(' ');
    }
    Serial.print('\n');
}

void communication::sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data)
{
    Serial.print(timestamp);
    Serial.print(' ');
    Serial.print(deviation);
    Serial.print(' ');
    for (auto i : raw_sample)
    {
        Serial.print(i);
        Serial.print(' ');
    }
    Serial.print('\n');
}

int communication::receiveData(std::array<uint8_t, 64> &rx_data)
{
    int n = 0;
    n = Serial.readBytes(rx_data.data(), 64);
    if (n < 64)
    {
        return 0; // not enough data received--we want the full packet
    }
    return n;
}
