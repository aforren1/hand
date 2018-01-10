#include <array>
#include "communication.hpp"

Serial.setTimeout(0); // TODO: not valid, we should just wrap this stuff up in a class

void communication::sendData(std::array<uint8_t, 64> &tx_data)
{
    Serial.write(tx_data.data(), 64); // TODO: append newline? The output won't be great in the Arduino IDE anyway...
    Serial.send_now();
}
void communication::sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
    communication::packGameSample(game_sample, tx_data);
    communication::sendData(tx_data);

}

void communication::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
}

void communication::sendSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data)
{
    communication::packRawSample(timestamp, deviation, raw_sample, tx_data);
    communication::sendData(tx_data);
}

void communication::packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample)
{
}

int communication::receiveData(std::array<uint8_t, 64> &rx_data)
{
    int n = 0;
    n = Serial.readBytes(rx_data.data(), 64);
    return 0;
}
