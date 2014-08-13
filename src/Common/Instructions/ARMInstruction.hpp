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
    BLX_IMM,
    BLX_REG,
    BX,
    CDP,
    CMN,
    CMP,
    EOR,
    LDC,
    LDM, // TODO: Multiple forms
    LDR,
    LDRB,
    LDRBT,
    LDRH,
    LDRSB,
    LDRSH,
    LDRT,
    MCR,
    MLA,
    MOV,
    MRC,
    MRS,
    MSR,
    MUL,
    MVN,
    ORR,
    RSB,
    RSC,
    SBC,
    SMLAL,
    SMULL,
    STC,
    STM, // TODO: Multiple forms
    STR,
    STRB,
    STRBT,
    STRH,
    STRT,
    SUB,
    SWI,
    SWP,
    SWPB,
    TEQ,
    TST,
    UMLAL,
    UMULL
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