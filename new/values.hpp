#pragma once

#include <cstdint>

namespace Values {

struct Number {
    enum {
        INT, UINT, FLOAT, STRING,
    } type;
    enum {
        DEC, // 1
        BIN, // 0b01
        OCT, // 0o01
        HEX, // 0x01

        HOCT, // 01
        HHEX, // h01

        DECH, // 01dec
        BINH, // 01bin
        OCTH, // 01oct
        HEXH, // 01h
        HEXH2, // 01hex

        FLOAT_EXP,      // 1e2
        FLOAT_INFO_HEX, // sign, mantissa, exponent
        FLOAT_INFO_DEC, // sign, mantissa, exponent

        STRING,
    } rep;
    enum {
        LITTLE, BIG,
        STRING,
    } endian;

    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;

        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;

        float f32;
        double f64;

        const char *string;
    } v;
};

}
