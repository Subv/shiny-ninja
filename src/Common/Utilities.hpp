#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <cstdint>
#include <limits>

// #undef _GLIBCXX_HAVE_BROKEN_VSWPRINTF

namespace Utilities
{
    void Assert(bool condition, const char* message);
}

class GeneralPurposeRegister
{
public:
    class BitReference
    {
    public:
        uint32_t* Reference;
        uint32_t Position;

        BitReference(GeneralPurposeRegister& reg, uint32_t position)
        {
            Reference = &reg.Value;
            Position = position;
        }

        BitReference& operator = (bool set)
        {
            if (set)
                *Reference |= 1 << Position;
            else
                *Reference &= ~(1 << Position);
            return *this;
        }

        BitReference& operator = (const BitReference& other)
        {
            if (*(other.Reference) & (1 << other.Position))
                *Reference |= (1 << Position);
            else
                *Reference &= ~(1 << Position);
            return *this;
        }

        bool operator ~ () const
        {
            return (*Reference & (1 << Position)) == 0;
        }

        operator bool() const { return ((*Reference) & (1 << Position)) != 0; }

        BitReference& Flip()
        {
            *Reference ^= 1 << Position;
            return *this;
        }
    };

    uint32_t Value = 0;
    bool Carry = false;
    bool Overflow = false;
    bool Borrow = false;

    operator uint32_t() const { return Value; }
    // Causes ambiguous casts (compiler boils down a bool() to `operator uint32_t() != 0`)
    explicit operator bool() const { return Value != 0; }

    BitReference operator[] (uint8_t i)
    {
        return BitReference(*this, i);
    }

    bool operator[] (uint8_t i) const
    {
        return (Value & (1 << i)) != 0;
    }

    uint32_t GetBits(uint8_t firstBitIndex, uint32_t bitCount);

    GeneralPurposeRegister& Flip()
    {
        Value = ~Value;
        return *this;
    }

    GeneralPurposeRegister& operator = (const uint64_t& val)
    {
        Borrow = false;
        Carry = false;
        Overflow = val > std::numeric_limits<int32_t>::max();
        Value = val;
        return *this;
    }

    GeneralPurposeRegister& operator = (const GeneralPurposeRegister& other)
    {
        Value = other.Value;
        Carry = other.Carry;
        Overflow = other.Overflow;
        Borrow = other.Borrow;
        return *this;
    }

    // Not needed due to the cast operator
    // bool operator != (uint32_t value) { return !(operator == (value)); }
    // bool operator == (uint32_t value) { return operator uint32_t() == value; }
    // bool operator < (unsigned int value) { return operator uint32_t() < value; }
    // bool operator <= (unsigned int value) { return operator uint32_t() <= value; }

    // Arithmetic operators
    GeneralPurposeRegister& operator += (uint32_t const& other)
    {
        int64_t dirtySum = (int64_t(Value) + other);

        Borrow = false;

        // Returns 1 if the addition specified as its parameter caused a carry
        // (true result is bigger than 2^32−1, where the operands are treated
        //  as unsigned integers), and returns 0 in all other cases.
        Carry = dirtySum > 0x80000000;

        // Returns 1 if the addition specified as its parameter caused a 32-bit
        // signed overflow. Addition generates an overflow if both operands have
        // the same sign (bit[31]), and the sign of the result is different to
        // the sign of both operands.
        Overflow = dirtySum > std::numeric_limits<uint32_t>::max();

        Value += other;
        return *this;
    }

    GeneralPurposeRegister& operator -= (uint32_t const& other)
    {
        Carry = false;
        Borrow = Value < other;
        Overflow = (uint64_t(Value) - other) < std::numeric_limits<uint32_t>::min();
        Value -= other;
        return *this;
    }

    GeneralPurposeRegister& operator *= (uint32_t const& other)
    {
        Value *= other;
        return *this;
    }

    GeneralPurposeRegister& operator %= (uint32_t const& other)
    {
        Value %= other;
        return *this;
    }

    // Bitwise operators
    GeneralPurposeRegister operator ~ () const
    {
        GeneralPurposeRegister copy;
        copy.Value = Value;
        return copy.Flip();
    }

    GeneralPurposeRegister& operator &= (uint32_t const& other)
    {
        Value &= other;
        return *this;
    }

    GeneralPurposeRegister& operator ^= (uint32_t const& other)
    {
        Value ^= other;
        return *this;
    }

    GeneralPurposeRegister& operator |= (uint32_t const& other)
    {
        Value |= other;
        return *this;
    }

    GeneralPurposeRegister& operator >>= (uint32_t const& other)
    {
        Value >>= other;
        return *this;
    }

    GeneralPurposeRegister& operator <<= (uint32_t const& other)
    {
        Value <<= other;
        return *this;
    }
};

inline GeneralPurposeRegister operator + (GeneralPurposeRegister const& left, GeneralPurposeRegister const& right)
{
    GeneralPurposeRegister reg = left;
    reg += right;
    return reg;
}

inline GeneralPurposeRegister operator - (GeneralPurposeRegister const& left, GeneralPurposeRegister const& right)
{
    GeneralPurposeRegister reg = left;
    reg -= right;
    return reg;
}

#endif
