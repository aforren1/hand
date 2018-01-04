#include "packing.hpp"

void packing::reverse_range(unsigned char buffer[], int left, int right)
{
    while (left < right)
    {
        int temp = buffer[left];
        buffer[left++] = buffer[right];
        buffer[right--] = temp;
    }
}

template <class T>
void packing::num2bigendbytes(T num, unsigned char b[sizeof(T)])
{
    memcpy(b, &num, sizeof(T));
    reverse_range(b, 0, sizeof(T) - 1);
}

template <class T>
T packing::bigendbytes2num(unsigned char b[sizeof(T)])
{
    T num;
    reverse_range(b, 0, sizeof(T) - 1);
    memcpy(&num, b, sizeof(T));
    return num;
}

extern template void packing::num2bigendbytes<int>(int num, unsigned char b[sizeof(int)]);
extern template void packing::num2bigendbytes<unsigned int>(unsigned int num, unsigned char b[sizeof(unsigned int)]);
extern template void packing::num2bigendbytes<long>(long num, unsigned char b[sizeof(long)]);
extern template void packing::num2bigendbytes<unsigned long>(unsigned long num, unsigned char b[sizeof(unsigned long)]);
extern template void packing::num2bigendbytes<float>(float num, unsigned char b[sizeof(float)]);

extern template int packing::bigendbytes2num(unsigned char b[sizeof(int)]);
extern template unsigned int packing::bigendbytes2num(unsigned char b[sizeof(unsigned int)]);
extern template long packing::bigendbytes2num(unsigned char b[sizeof(long)]);
extern template unsigned long packing::bigendbytes2num(unsigned char b[sizeof(unsigned long)]);
extern template float packing::bigendbytes2num(unsigned char b[sizeof(float)]);
