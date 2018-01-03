#ifndef HID_H
#define HID_H
#include <array>
#include "constants.hpp"

namespace hid
{
    void sendGameSample(const std::array<float, 15> &game_sample);
    void packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &packed_sample);
    void sendRawSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample);
    void packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample);
};

#endif
