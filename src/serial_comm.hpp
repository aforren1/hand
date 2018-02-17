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
 * @brief Sends a game sample to the host.
 * 
 * @param game_sample is a reference to a std::array of type float and length 15 (prepared by analog::applyRotation)
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 */
void sendSample(const std::array<float, 15> &game_sample);

/**
 * @brief Sends a raw sample to the host.
 * 
 * @param uint32_t timestamp is the time since the device powered on, in milliseconds.
 * @param int16_t deviation is the deviation from the expected sampling period, in microseconds.
 * @param raw_sample is a reference to a std::array of type uint16_t and length 20, which contains the "raw" (untransformed) analog readings.
 * @param tx_data is a reference to a std::array of type uint8_t and length 64 (the outgoing USB packet).
 * @return void
 */
void sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample);

/**
 * @brief Receive a packet from the host.
 * 
 * @param rx_data is a reference to a std::array of type uint8_t and length 64 (the incoming USB packet).
 * @return the number of bytes received as an integer
 */
int receiveRawPacket(std::array<uint8_t, 64> &rx_data);

/**
 * @brief Send a prepared packet to the host.
 *
 * @param tx_data is a reference to a std::array (the outgoing USB packet).
 */
void sendRawPacket(std::array<uint8_t, 64> &tx_data);

/**
 * @brief Send a std::string to the host.
 *
 * @param tx_string is a std::string of length <= 64.
 * @note We take care of padding to length 64 before sending (spaces and a newline).
 */
void sendString(std::string tx_string);

/**
 * @brief Concatenates two std::strings and sends to the host.
 *
 * @param string_1 First string.
 * @param string_2 second string.
 * @note communication::sendString("Optimization level ", std::to_string(3));
 */
void sendString(std::string string_1, std::string string_2);

/**
 * @brief Sends a raw sample to the host.
 *
 * @param err_sample is a reference to a std::array of type uint16_t and length 5, which contains the error sample.
 * @return void
 *
 * @note Error is calculated per-finger as abs(v[0] - v[1] + v[2] - v[3]), where v is an array of voltages.
 * @note This quantity is primarily used for debugging.
 */
void sendSample(const std::array<float, 5> &err_sample);
};

#endif
