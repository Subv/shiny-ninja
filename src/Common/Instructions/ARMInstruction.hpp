#ifndef ARMINSTRUCTION_HPP
#define ARMINSTRUCTION_HPP

#include "Instruction.hpp"

enum class InstructionCondition
{
    Equal, // EQ
    NotEqual, // NE
    CarrySet, // CS
    CarryCleared, // CC
    Negative, // MI
    PositiveOrZero, // PL
    Overflow, // VS
    NotOverflow, // VC
    UnsignedHigher, // HI
    UnsignedLowerOrSame, // LS
    GreaterEqual, // GE
    LessThan, // LT
    GreaterThan, // GT
    LessOrEqual, // LE
    Always, // AL
    Never // NV
};

class ARMInstruction : public Instruction
{
public:
    ARMInstruction() : Instruction() { }

    InstructionSet GetInstructionSet() override { return InstructionSet::ARM; }
    
    InstructionCondition GetCondition();
};
#endif