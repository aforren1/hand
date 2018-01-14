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
template <class T>
void num2bigendbytes(T num, std::array<uint8_t, sizeof(T)> &b);
template <class T>
T bigendbytes2num(std::array<uint8_t, sizeof(T)> &b);

template <class T>
void packing::num2bigendbytes(T num, std::array<uint8_t, sizeof(T)> &b)
{
    memcpy(b.data(), &num, sizeof(T));
    std::reverse(std::begin(b), std::end(b));
}

template <class T>
T packing::bigendbytes2num(std::array<uint8_t, sizeof(T)> &b)
{
    T num;
    std::reverse(std::begin(b), std::end(b));
    std::memcpy(&num, b.data(), sizeof(T));
    return num;
}
};

#endif
