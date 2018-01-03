#ifndef ENDIAN_H
#define ENDIAN_H
#include <array>

namespace endian {
    uint16_t swap_uint16(uint16_t val);
    int16_t swap_int16(int16_t val);
    uint32_t swap_uint32(uint32_t val);
    int32_t swap_int32(int32_t val);
};

#endif
