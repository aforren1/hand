#include <array>
#include "hid.hpp"

void hid::sendGameSample(const std::array<float, 15> &game_sample)
{
    
}

void hid::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &packed_sample)
{

}

void hid::sendRawSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample)
{

}

void hid::packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample)
{

}
