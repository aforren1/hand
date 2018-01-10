#include <array>
#include "communication.hpp"

void communication::_sendSample(std::array<uint8_t, 64> &tx_data)
{
    RawHID.send(tx_data.data(), 1);
}
void communication::sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
    communication::packGameSample(game_sample, tx_data);
    communication::_sendSample(tx_data);

}

void communication::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
}

void communication::sendSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data)
{
    communication::packRawSample(timestamp, deviation, raw_sample, tx_data);
    communication::_sendSample(tx_data);
}

void communication::packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample)
{
}

int communication::receiveData(std::array<uint8_t, 64> &rx_data)
{
    int code = RawHID.recv(rx_data.data(), 0);
    return code;
}
