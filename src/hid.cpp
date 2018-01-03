#include <array>
#include "hid.hpp"

void hid::sendGameSample(const unsigned long &timestamp, const std::array<float, 15> &game_sample)
{
    
}

void hid::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &packed_sample)
{

}

void hid::sendRawSample(const unsigned long &timestamp, const int &deviation, std::array<uint16_t, 20> &raw_sample)
{

}

void hid::packRawSample(const std::array<float, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample)
{

}
