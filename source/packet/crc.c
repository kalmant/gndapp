// Copyright (c) 2016-2019 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "crc.h"
#include <string.h>
#include <assert.h>

/**
 * @file
 * @brief CRC implementation.
 *
 * Generalized CRC-16 and CRC-32 algorithm, which can be
 * tuned to implement most of the CRC variants in use.
 *
 * This implementation is the variant without a lookup table, as the
 * primary aim is to run this code on microcontrollers, hence the table
 * is considered a "waste of memory".
 *
 * Acknowledgements:
 *
 * * Model of CRC by Ross N. Williams: https://www.zlib.net/crc_v3.txt
 * * Useful site for testing: https://crccalc.com/
 * * Bit reverse code copied from "Bit Twiddling Hacks" and "The Aggregate Magic Algorithms"
 *
 */

// CRC-16/ARC
#define CRC16_POLY 0x8005
#define CRC16_INIT 0x0000
#define CRC16_REF_IN 1
#define CRC16_REF_OUT 1
#define CRC16_XOR_OUT 0x0000

// CRC-32/ISO-HDLC (aka. "CRC-32")
#define CRC32_POLY 0x04C11DB7
#define CRC32_INIT 0xFFFFFFFF
#define CRC32_REF_IN 1
#define CRC32_REF_OUT 1
#define CRC32_XOR_OUT 0xFFFFFFFF

/**
 * @brief Reverses (or reflects) the bits in a byte.
 *
 * Source: "Bit Twiddling Hacks" http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
 */
static inline uint8_t bitreverse_8(uint8_t b)
{
#if defined(__arm__)
    uint32_t x = b;
    uint32_t result;
    __asm volatile ("RBIT %0, %1" : "=r" (result) : "r" (x) );
    return result >> 24;
#else
    return (uint8_t) (((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
#endif
}

/**
 * @brief Reverses (or reflects) the bits in a 32-bit number.
 *
 * Source: "The Aggregate Magic Algorithms" http://aggregate.org/MAGIC/#Bit%20Reversal
 */
static inline uint32_t bitreverse_32(uint32_t x)
{
#if defined(__arm__)
    uint32_t result;
    __asm volatile ("RBIT %0, %1" : "=r" (result) : "r" (x) );
    return result;
#else
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return ((x >> 16) | (x << 16));
#endif
}

/**
 * @brief Calculates CRC-16
 * @param input_bytes Input bytes
 * @param input_size Number of input bytes
 * @return The calculated CRC
 */
uint16_t crc16_calc(const uint8_t *input_bytes, size_t input_size)
{
    if (!input_size) {
        return 0;
    }

    assert(input_bytes);
    uint16_t out = CRC16_INIT;

    for (size_t byte_pos = 0; byte_pos < (input_size); ++byte_pos) {
        uint8_t in_byte = input_bytes[byte_pos];
        out ^= (CRC16_REF_IN ? bitreverse_8(in_byte) : in_byte) << 8;

        for (size_t bit_cnt = 0; bit_cnt < 8; ++bit_cnt) {
            uint32_t mask = -(out >> 15);
            out <<= 1;
            out ^= (CRC16_POLY & mask);
        }
    }

    if (CRC16_REF_OUT) {
        out = bitreverse_32(out) >> 16;
    }

    // Flip output bits
    out ^= CRC16_XOR_OUT;

    return out;
}

/**
 * @brief Calculates CRC-32
 * @param input_bytes Input bytes
 * @param input_size Number of input bytes
 * @return The calculated CRC
 */
uint32_t crc32_calc(const uint8_t *input_bytes, size_t input_size)
{
    if (!input_size) {
        return 0;
    }

    assert(input_bytes);
    uint32_t out = CRC32_INIT;

    for (size_t byte_pos = 0; byte_pos < (input_size); ++byte_pos) {
        uint8_t in_byte = input_bytes[byte_pos];
        out ^= (CRC32_REF_IN ? bitreverse_8(in_byte) : in_byte) << 24;

        for (size_t bit_cnt = 0; bit_cnt < 8; ++bit_cnt) {
            uint32_t mask = -(out >> 31);
            out <<= 1;
            out ^= (CRC32_POLY & mask);
        }
    }

    if (CRC32_REF_OUT) {
        out = bitreverse_32(out);
    }

    // Flip output bits
    out ^= CRC32_XOR_OUT;

    return out;
}
