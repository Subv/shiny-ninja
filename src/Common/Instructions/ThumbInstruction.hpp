#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"
#include "Common/Utilities.hpp"
#include <cstdint>

namespace Thumb
{
    // This may look super dumb, but there is no easy way to group these.
    // None that I can figure out, that is.
    enum ThumbOpcodes
    {
        ADC = 0,
        ADD_1,
        ADD_2,
        ADD_3,
        ADD_4,
        ADD_5,
        ADD_6,
        ADD_7,
        AND,
        ASR_1,
        ASR_2,
        B_1,
        B_2,
        BIC,
        BKPT,
        BL,
        BLX_1,
        BLX_2,
        BX,
        CMN,
        CMP_1,
        CMP_2,
        CMP_3,
        CPS,
        CPY,
        EOR,
        LDMIA,
        LDR_1,
        LDR_2,
        LDR_3,
        LDR_4,
        LDRB_1,
        LDRB_2,
        LDRH_1,
        LDRH_2,
        LDRSB,
        LDRSH,
        LSL_1,
        LSL_2,
        LSR_1,
        LSR_2,
        MOV_1,
        MOV_2,
        MOV_3,
        MUL,
        MVN,
        NEG,
        ORR,
        POP,
        PUSH,
        REV,
        REV16,
        REVSH,
        ROR,
        SBC,
        SETEND,
        STMIA,
        STR_1,
        STR_2,
        STR_3,
        STRB_1,
        STRB_2,
        STRH_1,
        STRH_2,
        SUB_1,
        SUB_2,
        SUB_3,
        SUB_4,
        SWI,
        SXTB,
        SXTH,
        TST,
        UXTB,
        UXTH
    };
    
    std::string ToString(uint32_t opcode);
}

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction(uint16_t instruction) : Instruction(instruction) { }
    InstructionSet GetInstructionSet() const override { return InstructionSet::Thumb; }
    
    virtual uint32_t GetOpcode() const override = 0;
    virtual std::string ToString() const override = 0;
    
    virtual bool IsImmediate() const override { return false; }
};
#endif
