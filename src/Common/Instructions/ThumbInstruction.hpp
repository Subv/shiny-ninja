#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"

enum ThumbOpcodes
{
    MOV,
    MVN,
    AND,
    TST,
    BIC,
    ORR,
    EOR,
    LSL,
    LSR,
    ASR,
    ROR,
    NOP,
    ADD,
    ADC,
    SUB,
    SBC,
    NEG,
    CMP,
    CMN,
    MUL,
    B,
    BL,
    BX,
    SWI,
    BKPT,
    BLX,
    POP,
    LDR,
    LDRB,
    LDRH,
    LDSB,
    LDSH,
    STR,
    STRB,
    STRH,
    PUSH,
    STMIA,
    LDMIA
};

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction(uint16_t instruction) : Instruction(instruction) { }
    InstructionSet GetInstructionSet() override { return InstructionSet::Thumb; }
};
#endif