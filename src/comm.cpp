#include <Arduino.h> // necessary for RawHID invocations
#undef min
#undef max
#include "packing.hpp"
#include <array>
#include <algorithm>
#include <string>
#include "comm.hpp"

void comm::setupComm() {}

void comm::packGameSample(const std::array<float, 15> &game_sample, std::array<uint8_t, 64> &tx_data)
{
    std::array<uint8_t, 4> flt_container;
    unsigned int count = 0;
    for (auto i : game_sample)
    {
        packing::num2bigendbytes<float>(i, flt_container);
        std::copy_n(flt_container.begin(), flt_container.size(), tx_data.begin() + count);
        count += 4;
    }
    // at this point, tx_data should have 60 bytes filled
}

void comm::sendSample(const std::array<float, 15> &game_sample)
{
    std::array<uint8_t, 64> tx_data;
    comm::packGameSample(game_sample, tx_data);
    RawHID.send(tx_data.data(), 1);
}

// TODO: Below is a prime candidate for templates
void comm::packErrSample(const std::array<float, 5> &err_sample, std::array<uint8_t, 64> &tx_data)
{
    std::array<uint8_t, 4> flt_container;
    unsigned int count = 0;
    for (auto i : err_sample)
    {
        packing::num2bigendbytes<float>(i, flt_container);
        std::copy_n(flt_container.begin(), flt_container.size(), tx_data.begin() + count);
        count += 4;
    }
    // at this point, tx_data should have 60 bytes filled
}

void comm::sendSample(const std::array<float, 5> &err_sample)
{
    std::array<uint8_t, 64> tx_data;
    comm::packErrSample(err_sample, tx_data);
    RawHID.send(tx_data.data(), 1);
}

void comm::packRawSample(uint32_t timestamp, int16_t deviation, const std::array<uint16_t, 20> &raw_sample, std::array<uint8_t, 64> &tx_data)
{
    std::array<uint8_t, 4> long_container;
    std::array<uint8_t, 2> int_container;
    packing::num2bigendbytes<uint32_t>(timestamp, long_container);
    packing::num2bigendbytes<int16_t>(deviation, int_container);
    std::copy_n(long_container.begin(), long_container.size(), tx_data.begin());
    std::copy_n(int_container.begin(), int_container.size(), tx_data.begin() + 4);
    unsigned int count = 6;
    for (auto i : raw_sample)
    {
        packing::num2bigendbytes<unsigned short>(i, int_container);
        std::copy_n(int_container.begin(), int_container.size(), tx_data.begin() + count);
        count += 2;
    }
    // should have 46 bytes filled by now
}

void comm::sendSample(uint32_t timestamp, int16_t deviation, std::array<uint16_t, 20> &raw_sample)
{
    std::array<uint8_t, 64> tx_data;
    comm::packRawSample(timestamp, deviation, raw_sample, tx_data);
    RawHID.send(tx_data.data(), 1);
}

int comm::receiveRawPacket(std::array<uint8_t, 64> &rx_data)
{
    int code = RawHID.recv(rx_data.data(), 0);
    return code;
}

void comm::sendRawPacket(std::array<uint8_t, 64> &tx_data)
{
    RawHID.send(tx_data.data(), 1);
}

void comm::sendString(std::string &tx_string)
{
    tx_string.append(63 - tx_string.length(), ' ');
    tx_string.append("\n");
    Serial.write(tx_string.c_str(), 64);
    Serial.send_now();
}

void comm::sendString(std::string &string_1, std::string &string_2)
{
    std::string x = string_1 + string_2;
    comm::sendString(x);
}
