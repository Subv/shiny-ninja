#ifndef MATH_HPP
#define MATH_HPP

#include "Utilities.hpp"

#include <cstdint>
#include <type_traits>
#include <limits>

#ifndef CHAR_BIT // Defined in <limits.h>, buuuut we already have <limits>
# define CHAR_BIT 8 // Let's just assume that everyone has 8-bit wide chars nowadays...
#endif

#define SignExtend(x) ((int32_t)((int8_t)x))

namespace MathHelper
{
    bool CheckBit(uint64_t val, uint8_t bit);

    bool CheckBits(uint32_t value, uint8_t start, uint8_t bitCount, uint32_t check);

    uint32_t GetBits(uint64_t value, uint8_t start, uint8_t bitCount);

    template <typename T>
    inline T RotateLeft(T val, uint8_t moves)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
        return (val << moves) | (val >> (sizeof(T) * CHAR_BIT - moves));
    }

    //! TODO Optimize it for the compiler. This probably won't inline
    template<>
    inline GeneralPurposeRegister RotateLeft<GeneralPurposeRegister>(GeneralPurposeRegister val, uint8_t moves)
    {
        GeneralPurposeRegister reg;
        reg.Value = (val << moves) | (val >> (32 - moves));
        return reg;
    }

    template<typename T>
    inline T RotateRight(T x, uint8_t moves)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
        return (x >> moves) | (x << (sizeof(T) * CHAR_BIT - moves));
    }

    //! TODO Optimize it for the compiler. This probably won't inline
    template<>
    inline GeneralPurposeRegister RotateRight<GeneralPurposeRegister>(GeneralPurposeRegister x, uint8_t moves)
    {
        GeneralPurposeRegister reg;
        reg.Value = (x >> moves) | (x << (32 - moves));
        return reg;
    }

    template<typename T>
    inline bool Overflow(int64_t value)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
        return value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max();
    }

    inline bool BorrowFrom(int32_t left, int32_t right)
    {
        return uint32_t(left) < uint32_t(right);
    }

    // OK
    inline bool CarryFrom(int32_t left, int32_t right)
    {
        return uint32_t(0xFFFFFFFFU - uint32_t(left)) < uint32_t(right);
    }

    // OK
    inline bool CarryFrom16(int16_t left, int16_t right)
    {
        return uint16_t(0xFFFFU - uint16_t(left)) < uint16_t(right);
    }

    inline uint32_t NumberOfSetBits(uint32_t i)
    {
        i = i - ((i >> 1) & 0x55555555);
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
        return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }

    template<int SourceBits, int DestBits>
    inline int32_t IntegerSignExtend(uint32_t num)
    {
        return (int32_t(num) << (DestBits - SourceBits)) >> (DestBits - SourceBits);
    }
}
#endif
