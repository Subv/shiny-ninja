#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <bitset>
#include <cstdint>

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

    uint32_t Value;
    // bool Carry = false;
    // bool Overflow = false;
    // bool Borrow = false;

    operator uint32_t() const { return Value; }
    // Causes ambiguous casts (compiler boils down a bool() to `operator uint32_t() != 0`)
    // operator bool() const { return Value != 0; }

    BitReference operator[] (uint8_t i)
    {
        return BitReference(*this, i);
    }

    bool operator[] (uint8_t i) const
    {
        return (Value & (1 << i)) != 0;
    }

    uint32_t GetBits(uint8_t firstBitIndex, uint32_t bitCount)
    {
        if (firstBitIndex + bitCount > 32)
            bitCount = 32 - firstBitIndex;
        uint32_t returnValue = 0;
        for (; bitCount > 0; --bitCount)
            returnValue |= uint32_t(operator[](bitCount + firstBitIndex)) << bitCount;
        return returnValue;
    }

    GeneralPurposeRegister& Flip()
    {
        Value = ~Value;
        return *this;
    }

    GeneralPurposeRegister& operator = (const uint32_t& val)
    {
        Value = val;
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
        Value += other;
        return *this;
    }

    GeneralPurposeRegister& operator -= (uint32_t const& other)
    {
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

#endif
