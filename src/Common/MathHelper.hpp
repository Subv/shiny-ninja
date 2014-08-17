#ifndef MATH_HPP
#define MATH_HPP

#include <cstdint>
#include <type_traits>
#include <limits>
#include <strings.h>

#ifndef CHAR_BIT // Defined in <limits.h>, buuuut we already have <limits>
# define CHAR_BIT sizeof(char)
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
}

template<char... D>
struct ToBinary;

template<char... D> constexpr int operator "" _b()
{
    return ToBinary<D...>::value;
}

template<char H, char... D> struct ToBinary<H, D...>
{
    static_assert(H == '0' || H == '1', "Non-bit value passed!");
    static int const value = (H - '0') * (1 << sizeof...(D)) + 
                             ToBinary<D...>::value;
};

template<char H> struct ToBinary<H>
{
    static_assert(H == '0' || H == '1', "Non-bit value passed!");
    static int const value = (H - '0');
};

#endif