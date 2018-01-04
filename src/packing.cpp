#include <iostream>
#include <array>
#include <algorithm>
#include <cstring>

#include "packing.hpp"

template <class T>
void num2bigendbytes(T num, std::array<unsigned char, sizeof(T)> &b)
{
    memcpy(b.data(), &num, sizeof(T));
    std::reverse(std::begin(b), std::end(b));
}

template <class T>
T bigendbytes2num(std::array<unsigned char, sizeof(T)> &b)
{
    T num;
    std::reverse(std::begin(b), std::end(b));
    std::memcpy(&num, b.data(), sizeof(T));
    return num;
}

extern template void packing::num2bigendbytes<int>(int num, std::array<unsigned char, sizeof(int)> &b);
extern template void packing::num2bigendbytes<unsigned int>(unsigned int num, std::array<unsigned char, sizeof(unsigned int)> &b);
extern template void packing::num2bigendbytes<long>(long num, std::array<unsigned char, sizeof(long)> &b);
extern template void packing::num2bigendbytes<unsigned long>(unsigned long num, std::array<unsigned char, sizeof(unsigned long)> &b);
extern template void packing::num2bigendbytes<float>(float num, std::array<unsigned char, sizeof(float)> &b);

extern template int packing::bigendbytes2num(std::array<unsigned char, sizeof(int)> &b);
extern template unsigned int packing::bigendbytes2num(std::array<unsigned char, sizeof(unsigned int)> &b);
extern template long packing::bigendbytes2num(std::array<unsigned char, sizeof(long)> &b);
extern template unsigned long packing::bigendbytes2num(std::array<unsigned char, sizeof(unsigned long)> &b);
extern template float packing::bigendbytes2num(std::array<unsigned char, sizeof(float)> &b);
