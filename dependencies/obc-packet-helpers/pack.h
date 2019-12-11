
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

#ifndef S1_PACK_H
#define S1_PACK_H

#include <cstdint>
#include <cstring>
#include <type_traits>

/**
 * @file
 * @brief Header that contains the Pack template implementation.
 */

namespace s1utils {

    namespace detail {

        /**
         * @brief Helps to identify a type.
         *
         * It is typically used by variadic templates to identify
         * a template type parameter inside a variadic template parameter pack.
         */
        template <typename T>
        struct Identify
        {
            using Type = T;
        };

        /**
         * @brief Chooses the Nth element of a parameter pack.
         *
         * It is typically used by variadic templates to choose
         * the Nth type parameter inside a variadic template parameter pack.
         */
        template <size_t N, typename... T>
        struct Choose;

        template <size_t N, typename T0, typename... T>
        struct Choose<N, T0, T...> : Choose<N - 1, T...>
        {
        };

        template <typename T0, typename... T>
        struct Choose<0, T0, T...> : Identify<T0>
        {
        };

        /**
         * @brief Calculates the total size of a parameter pack.
         *
         * It is typically used by variadic templates to calculate
         * the total size (in bytes) of all the parameters in a variadic
         * template parameter pack.
         */
        template <typename... TArgs>
        struct SizeSum;

        /**
         * @brief Helper used by `SizeSum` for identification.
         *
         * Used by `SizeSum` to identify one of its specializations.
         * It works on a similar principle as `Identify`.
         */
        template <typename... TArgs>
        struct IdentifySizeSum
        {
            using Type = SizeSum<TArgs...>;
        };

        /**
         * @brief Helper used by `SizeSum` for choosing.
         *
         * Used by `SizeSum` to choose the one of its specializations
         * with the first N template parameters.
         * It works on a similar principle as `Choose`, but is more complicated.
         */
        template <size_t N, typename... T>
        struct ChooseSizeSum;

        template <size_t N, typename T0, typename... TOthers>
        struct ChooseSizeSum<N, T0, TOthers...> : ChooseSizeSum<N - 1, TOthers...>
        {
        };

        template <typename T0, typename... TOthers>
        struct ChooseSizeSum<0, T0, TOthers...> : IdentifySizeSum<T0, TOthers...>
        {
        };

        template <typename T0, typename... TOthers>
        struct SizeSum<T0, TOthers...>
        {
            static constexpr size_t size = sizeof(T0) + SizeSum<TOthers...>::size;
            static constexpr size_t offset = size - sizeof(T0);
        };

        template <typename T>
        struct SizeSum<T>
        {
            static constexpr size_t size = sizeof(T);
            static constexpr size_t offset = size - sizeof(T);
        };

    } // namespace detail

    /**
     * @brief Container for packed data.
     *
     * Intended for simple binary protocols for use instead of packed structs.
     */
    template <typename... TPackArgs>
    class Pack
    {

    private:
        template <typename T, size_t byteIndex>
        static T getData(const uint8_t *bytes)
        {
            T result;
            ::std::memcpy(&result, bytes + byteIndex, sizeof(T));
            return result;
        }

        /**
         * @brief Gets a reference to some data inside the given array with the given type.
         * @warning On ARM and other architectures that don't support unaligned access,
         *          this will only work reliably with one-byte data types.
         */
        template <typename T, size_t byteIndex>
        static T &getDataRef(uint8_t *bytes)
        {
            static_assert(
                sizeof(T) == sizeof(uint8_t),
                "Only one-byte data types are allowed with Pack::getDataRef because otherwise it might cause unaligned access issues.");
            return reinterpret_cast<T &>(bytes[byteIndex]);
        }

        /**
         * @brief Gets a const reference to some data inside the given array with the given type.
         * @warning On ARM and other architectures that don't support unaligned access,
         *          this will only work reliably with one-byte data types.
         */
        template <typename T, size_t byteIndex>
        static const T &getDataRef(const uint8_t *bytes)
        {
            static_assert(
                sizeof(T) == sizeof(uint8_t),
                "Only one-byte data types are allowed with Pack::getDataRef because otherwise it might cause unaligned access issues.");
            return reinterpret_cast<const T &>(bytes[byteIndex]);
        }

        template <typename T, size_t byteIndex>
        static void setData(uint8_t *bytes, const T &item)
        {
            ::std::memcpy(bytes + byteIndex, &item, sizeof(T));
        }

        template <typename T0>
        static void initializeData(uint8_t *bytes, const T0 &item0)
        {
            Pack::setData<T0, 0>(bytes, item0);
        }

        template <typename T0, typename... TOthers>
        static void initializeData(uint8_t *bytes, const T0 &item0, const TOthers &... others)
        {
            Pack::setData<T0, detail::SizeSum<T0, TOthers...>::offset>(bytes, item0);
            Pack::initializeData<TOthers...>(bytes, others...);
        }

        static constexpr size_t m_fullSize = detail::SizeSum<TPackArgs...>::size;
        uint8_t m_bytes[m_fullSize];

    public:
        template <size_t N>
        using ItemType = typename detail::Choose<N, TPackArgs...>::Type;

        template <size_t N>
        using ItemSize = typename detail::ChooseSizeSum<N, TPackArgs...>::Type;

        explicit Pack() { memset(this->m_bytes, 0, Pack::m_fullSize); }

        explicit Pack(const TPackArgs &... args) { Pack::initializeData<TPackArgs...>(m_bytes, args...); }

        void loadBinary(const uint8_t *binary) { ::std::memcpy(this->m_bytes, binary, sizeof(this->m_bytes)); }

        const uint8_t *binary() const { return this->m_bytes; }

        uint8_t *binary() { return this->m_bytes; }

        template <size_t N>
        inline ItemType<N> get() const
        {
            static_assert(std::is_trivially_copyable<ItemType<N>>::value, "Item type must be trivially copyable.");
            return Pack::getData<ItemType<N>, m_fullSize - ItemSize<N>::offset - sizeof(ItemType<N>)>(m_bytes);
        }

        template <size_t N>
        inline void set(const ItemType<N> &item)
        {
            static_assert(std::is_trivially_copyable<ItemType<N>>::value, "Item type must be trivially copyable.");
            return Pack::setData<ItemType<N>, m_fullSize - ItemSize<N>::offset - sizeof(ItemType<N>)>(m_bytes, item);
        }

        template <size_t N>
        inline ItemType<N> &getRef()
        {
            return Pack::getDataRef<ItemType<N>, m_fullSize - ItemSize<N>::offset - sizeof(ItemType<N>)>(m_bytes);
        }

        template <size_t N>
        inline const ItemType<N> &getRef() const
        {
            return Pack::getDataRef<ItemType<N>, m_fullSize - ItemSize<N>::offset - sizeof(ItemType<N>)>(m_bytes);
        }

        template <size_t N, uint32_t NBitfield>
        inline uint8_t getBitfield8() const
        {
            auto &bitfield8 = this->getRef<N>();
            return bitfield8.template get<NBitfield>();
        }

        template <size_t N, uint32_t NBitfield>
        inline void setBitfield8(uint8_t value)
        {
            auto &bitfield8 = this->getRef<N>();
            bitfield8.template set<NBitfield>(value);
        }

    }; // class Pack

} // namespace s1utils

#endif // S1_PACK_H
