#ifndef PACKING_H
#define PACKING_H
#include <iostream>
#include <array>
#include <algorithm>
#include <cstring>

namespace packing
{
// these methods are for converting int/float/etc to uint8_t arrays
// *plus* doing the endianness handling.

/**
 * @brief function template that handles conversion from type T to an array of bytes.
 * 
 * @param num is the number to be converted (typically an int or float)
 * @param b is a reference to a std::array with a number of elements to match the size of num's type.
 * @return void
 * 
 * @note Also handles endianness (hosts are expecting big-endian numbers)
 * @note See usage in hid_comm.cpp
 */
template <class T>
void num2bigendbytes(T num, std::array<uint8_t, sizeof(T)> &b);

/**
 * @brief function template that handles conversion from array of bytes to a value of type T.
 * 
 * @param b is a reference to a std::array with a number of elements to match the size of the output type.
 * @return value of type T
 * 
 * @note Also handles endianness (hosts are expected to send big-endian numbers)
 * @note See usage in ui.cpp
 */
template <class T>
T bigendbytes2num(std::array<uint8_t, sizeof(T)> &b);

template <class T>
void num2bigendbytes(T num, std::array<uint8_t, sizeof(T)> &b)
{
    memcpy(b.data(), &num, sizeof(T));
    std::reverse(std::begin(b), std::end(b));
}

template <class T>
T bigendbytes2num(std::array<uint8_t, sizeof(T)> &b)
{
    T num;
    std::reverse(std::begin(b), std::end(b));
    std::memcpy(&num, b.data(), sizeof(T));
    return num;
}
};

#endif
