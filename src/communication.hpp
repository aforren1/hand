#ifndef COMM_H
#define COMM_H
#include <array>
#include "constants.hpp"

namespace communication
{
void initComm();
void sendData(std::array<uint8_t, 64> &tx_data);
void packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);
void sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);
void packRawSample(unsigned long timestamp, int deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);
void sendSample(unsigned long timestamp, int deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);
int receiveData(std::array<uint8_t, 64> &rx_data);
};

#endif
