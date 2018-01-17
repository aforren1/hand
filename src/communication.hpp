#ifndef COMM_H
#define COMM_H
#include <array>

namespace communication
{
void initComm();
void sendData(std::array<uint8_t, 64> &tx_data);
void packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);
void sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);
void packRawSample(uint32_t timestamp, int16_t deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);
void sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);
int receiveData(std::array<uint8_t, 64> &rx_data);
};

#endif
