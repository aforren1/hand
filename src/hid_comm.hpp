#ifndef COMM_H
#define COMM_H
#include <array>

namespace communication
{
/**
 * @brief Initialize the communication method.
 * 
 * @return void
 * 
 * @note not always necessary (e.g. for HID version)
 */
void initComm();

/**
 * @brief Stores the game sample as a series of bytes (uint8_t) for transfer to the host.
 * 
 * @param game_sample is a reference to a std::array of type float and length 15 (prepared by analog::applyRotation)
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 * 
 * @note Teensy (and other microcontrollers) tend to be little-endian, and we want to communicate as big-endian.
 * @note This function ends up handling endianness issues via packing::num2bigendbytes.
 */
void packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);

/**
 * @brief Sends a game sample to the host.
 * 
 * @param game_sample is a reference to a std::array of type float and length 15 (prepared by analog::applyRotation)
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 */
void sendSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data);

/**
 * @brief Stores the raw sample as a series of bytes (uint8_t) for transfer to the host.
 * 
 * @param uint32_t timestamp is the time since the device powered on, in milliseconds.
 * @param int16_t deviation is the deviation from the expected sampling period, in microseconds.
 * @param raw_sample is a reference to a std::array of type uint16_t and length 20, which contains the "raw" (untransformed) analog readings.
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 */
void packRawSample(uint32_t timestamp, int16_t deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);

/**
 * @brief Sends a raw sample to the host.
 * 
 * @param uint32_t timestamp is the time since the device powered on, in milliseconds.
 * @param int16_t deviation is the deviation from the expected sampling period, in microseconds.
 * @param raw_sample is a reference to a std::array of type uint16_t and length 20, which contains the "raw" (untransformed) analog readings.
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 */
void sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data);

/**
 * @brief Receive a packet from the host.
 * 
 * @param rx_data is a reference to a std::array of type uint8_t and length 64 (the incoming USB packet).
 * @return the number of bytes received as an integer
 */
int receiveData(std::array<uint8_t, 64> &rx_data);

void sendInfo(std::array<uint8_t, 64> &tx_data);

void sendString(std::string tx_string);
};

#endif
