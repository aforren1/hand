#ifndef PACKING_H
#define PACKING_H
#include <array>

namespace packing {
    // these methods are for converting int/float/etc to uint8_t arrays
    // *plus* doing the endianness handling.
    void pack_float(float val, uint8_t buf[]);
    float unpack_float(uint8_t buf[]); // should 
    void pack_uint16(uint16_t val, uint8_t buf[]);
    uint16_t unpack_uint16(uint8_t buf[]);
    void pack_int16(int16_t val, uint8_t buf[]);
    int16_t unpack_int16(uint8_t buf[]);
    void pack_uint32(uint32_t val, uint8_t buf[]);
    uint32_t unpack_uint32(uint8_t buf[]);
    void pack_int32(int32_t val, uint8_t buf[]);
    int32_t unpack_int32(uint8_t buf[]);
};

#endif
