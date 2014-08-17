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

namespace ARM
{
    enum class ShiftType
    {
        LSL,
        LSR,
        ASR,
        ROR
    };

    enum ARMOpcodes
    {
        ADC,
        ADD,
        AND,
        B,
        BL,
        BIC,
        BLX,
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
    
    std::string ToString(uint32_t opcode);
}

class ARMInstruction : public Instruction
{
public:
    ARMInstruction(uint32_t instruction) : Instruction(instruction) { }

    InstructionSet GetInstructionSet() const override { return InstructionSet::ARM; }
    
    InstructionCondition GetCondition() const;

    virtual uint32_t GetOpcode() const override = 0;
    virtual std::string ToString() const override = 0;
    virtual bool IsImmediate() const { return false; }
};
#endif