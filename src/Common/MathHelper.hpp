#ifndef MATH_HPP
#define MATH_HPP

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

    template<typename T>
    inline T RotateRight(T x, uint8_t moves)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
        return (x >> moves) | (x << (sizeof(T) * CHAR_BIT - moves));
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

    inline bool CarryFrom(int32_t left, int32_t right)
    {
        return uint32_t(0xFFFFFFFFU - uint32_t(left)) < uint32_t(right);
    }

    template <bool isAddition>
    inline bool OverflowFrom(int32_t left, int32_t right)
    {
        int32_t result = isAddition ? (left + right) : (left - right);
        if (isAddition)
            return ((left >= 0 && right >= 0) || (left < 0 && right < 0)) && ((left < 0 && result >= 0) || (left >= 0 && result < 0));
        return ((left < 0 && right >= 0) || (left >= 0 && right < 0)) && ((left < 0 && result >= 0) || (left >= 0 && result < 0));
    }

    inline uint32_t NumberOfSetBits(uint32_t i)
    {
        i = i - ((i >> 1) & 0x55555555);
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
        return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }
}
#endif
