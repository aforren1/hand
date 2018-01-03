#include "constants.hpp"

namespace hid
{
    void sendGameSample(const unsigned long &timestamp, const std::array<float, 15> &game_sample);
    void packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &packed_sample);
    void sendRawSample(const unsigned long &timestamp, const int &deviation, std::array<uint16_t, 20> &raw_sample);
    void packRawSample(const std::array<float, 20> &raw_sample, std::array<uint8_t, 64> &packed_sample);
};
