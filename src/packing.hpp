#ifndef PACKING_H
#define PACKING_H
#include <array>

namespace packing
{
// these methods are for converting int/float/etc to uint8_t arrays
// *plus* doing the endianness handling.
template <class T>
void num2bigendbytes(T num, std::array<unsigned char, sizeof(T)> &b);
template <class T>
T bigendbytes2num(std::array<unsigned char, sizeof(T)> &b);
};

#endif
