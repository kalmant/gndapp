
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

#ifndef S1_BITFIELD_H
#define S1_BITFIELD_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

/**
 * @file
 * @brief Header that contains the Bitfield template implementation.
 */

namespace s1utils {

    namespace detail {

        /**
         * @brief Helper to create bit field positions
         */
        template <uint32_t X0, uint32_t... X>
        struct BitfieldPosition
        {
            static constexpr uint32_t position = BitfieldPosition<X...>::totalSize;
            static constexpr uint32_t size = X0;
            static constexpr uint32_t totalSize = position + size;
        };

        template <uint32_t X0>
        struct BitfieldPosition<X0>
        {
            static constexpr uint32_t position = 0;
            static constexpr uint32_t size = X0;
            static constexpr uint32_t totalSize = position + size;
        };

        /**
         * @brief Helper for identifying a `BitfieldPosition`.
         */
        template <uint32_t... X>
        struct IdentifyBitfieldPosition
        {
            using Type = BitfieldPosition<X...>;
        };

        /**
         * @brief Helper for choosing a correct `BitfieldPosition` for a bitfield member.
         */
        template <uint32_t N, uint32_t X0, uint32_t... X>
        struct ChooseBitfieldPosition : ChooseBitfieldPosition<N - 1, X...>
        {
        };

        template <uint32_t X0, uint32_t... X>
        struct ChooseBitfieldPosition<0, X0, X...> : IdentifyBitfieldPosition<X0, X...>
        {
        };

        /**
         * @brief Helper for creating a sequence of one bits.
         */
        template <uint32_t BitCount>
        struct OneBits
        {
            static constexpr uint32_t bits = ((OneBits<BitCount - 1>::bits << 1) | 1);
        };

        template <>
        struct OneBits<1>
        {
            static constexpr uint32_t bits = 1;
        };

    } // namespace detail

    template <typename T, uint32_t... X>
    class Bitfield
    {
    public:
        using BasicType = T;

        explicit Bitfield(T data = 0)
            : m_data(data)
        {
        }

        Bitfield &operator=(T data)
        {
            this->m_data = data;
            return *this;
        }

        inline operator T() const
        {
            // Just return the data
            return m_data;
        }

        inline void load(T data)
        {
            this->m_data = data;
        }

        template <uint32_t N>
        inline T get() const
        {
            constexpr uint32_t shiftLTH = bitcount - Position<N>::position - Position<N>::size;
            constexpr uint32_t shiftHTL = Position<N>::position;
            constexpr uint32_t shift = bitOrderLowToHigh ? shiftLTH : shiftHTL;
            constexpr uint32_t mask = Mask<N>::bits;

            return this->getBits<shift, mask>();
        }

        template <uint32_t N>
        inline void set(T val)
        {
            constexpr uint32_t shiftLTH = bitcount - Position<N>::position - Position<N>::size;
            constexpr uint32_t shiftHTL = Position<N>::position;
            constexpr uint32_t shift = bitOrderLowToHigh ? shiftLTH : shiftHTL;
            constexpr uint32_t mask = Mask<N>::bits;

            return this->setBits<shift, mask>(val);
        }

    private:
        /**
         * @brief Gets bits from the bit field.
         * @tparam rightShift Amount of right shift applied to the data.
         * @tparam mask Mask applied to the data after the right shift.
         */
        template <T rightShift, T mask>
        inline T getBits() const
        {
            // Get relevant bits
            T bits = this->m_data >> rightShift;
            // Apply mask
            bits &= mask;

            return bits;
        }

        /**
         * @brief Sets bits into the bit field.
         * @tparam leftShift Amount of left shift applied to the data.
         * @tparam mask Mask applied to the data before the left shift.
         */
        template <T leftShift, T mask>
        inline void setBits(T val)
        {
            // Clear relevant bits
            this->m_data &= ~(mask << leftShift);
            // Set new value
            this->m_data |= ((val & mask) << leftShift);
        }

        template <uint32_t N>
        using Position = typename detail::ChooseBitfieldPosition<N, X...>::Type;

        template <uint32_t N>
        using Mask = typename detail::OneBits<Position<N>::size>;

        static constexpr uint32_t bitcount = sizeof(T) * 8;
        static constexpr uint32_t bitOrderLowToHigh = false;

    private:
        T m_data;
    };

    static_assert(sizeof(Bitfield<uint8_t, 8>) == sizeof(uint8_t), "Size of a one-byte Bitfield should be one byte.");

} // namespace s1utils

#endif // S1_BITFIELD_H
