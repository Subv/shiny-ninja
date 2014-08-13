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
    Unused
};

enum ARMOpcodes
{
    ADC,
    ADD,
    AND,
    B,
    BL,
    BIC,
    BKPT,
    BLX_IMM,
    BLX_REG,
    BX,
    CDP,
    CLZ,
    CMN,
    CMP,
    CPS,
    CPY,
    EOR,
    LDC
};

class ARMInstruction : public Instruction
{
public:
    ARMInstruction(uint32_t instruction) : Instruction(instruction) { }

    InstructionSet GetInstructionSet() override { return InstructionSet::ARM; }
    
    InstructionCondition GetCondition();

    std::string ToString() override = 0;
};
#endif