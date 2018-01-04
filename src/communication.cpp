#include <array>
#include "communication.hpp"

void communication::sendGameSample(const std::array<float, 15> &game_sample)
{
}

void communication::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &packed_sample)
{
}

void communication::sendRawSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample)
{
}

void communication::packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample)
{
}

int communication::receiveData(std::array<uint8_t, 64> &rx_data)
{
    int code = RawHID.recv(rx_data.data(), 0);
    return code;
}
