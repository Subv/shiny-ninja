#ifndef MATH_HPP
#define MATH_HPP

#include <cstdint>
#include <type_traits>
#include <limits>

#ifndef CHAR_BIT // Defined in <limits.h>, buuuut we already have <limits>
# define CHAR_BIT 8
#endif

namespace MathHelper
{
    bool CheckBit(uint64_t val, uint8_t bit);

    bool CheckBits(uint32_t value, uint8_t start, uint8_t bitCount, uint32_t check);

    uint32_t GetBits(uint32_t value, uint8_t start, uint8_t bitCount);

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

    template<typename T>
    inline bool BorrowFrom(int64_t left, int64_t right)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
        return int64_t(left) - int64_t(right) < 0;
    }
}
#endif
