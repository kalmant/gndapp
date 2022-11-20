
// This file is part of the SMOG-1 Utilities library.
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

#ifndef S1_INT24_H
#define S1_INT24_H

#include <cassert>
#include <cstddef>
#include <cstdint>

/**
 * @file
 * @brief Header that contains 24-bit integer types.
 */

namespace s1utils {

    /**
     * @brief Unsigned 24-bit integer type.
     */
    class s1_uint24_t final {

    private:
        uint8_t m_bytes[3];

    public:
        inline s1_uint24_t() : m_bytes{0, 0, 0} {
        }

        inline s1_uint24_t(uint32_t u32) {
            this->operator=(u32);
        }

        inline s1_uint24_t(const s1_uint24_t &other) = default;

        inline s1_uint24_t &operator=(const s1_uint24_t &other) = default;

        s1_uint24_t &operator=(uint32_t u32) {
            assert(u32 <= 0x00ffffff);

            // Chop off the MSB
            u32 &= 0x00ffffff;

            this->m_bytes[0] = (u32 & 0x00ff0000) >> 16;
            this->m_bytes[1] = (u32 & 0x0000ff00) >> 8;
            this->m_bytes[2] = (u32 & 0x000000ff) >> 0;

            return *this;
        }

        operator uint32_t() const {
            uint32_t result = 0;

            result |= this->m_bytes[0];
            result <<= 8;
            result |= this->m_bytes[1];
            result <<= 8;
            result |= this->m_bytes[2];

            return result;
        }

        void setByte(size_t index, uint8_t byte) {
            assert(index < 3);
            m_bytes[index] = byte;
        }
    } __attribute__((packed));

    static_assert(3 == sizeof(s1_uint24_t), "s1_uint24_t size MUST BE 24 bits, ie. 3 bytes.");

} // namespace s1utils

#endif // S1_INT24_H
