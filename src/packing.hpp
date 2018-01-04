#ifndef PACKING_H
#define PACKING_H

namespace packing {
    // these methods are for converting int/float/etc to uint8_t arrays
    // *plus* doing the endianness handling.
    void reverse_range(unsigned char buffer[], int left, int right);
    template <class T> void num2bigendbytes(T num,  unsigned char b[sizeof(T)]);
    template <class T> T bigendbytes2num(unsigned char b[sizeof(T)]);

};

#endif
