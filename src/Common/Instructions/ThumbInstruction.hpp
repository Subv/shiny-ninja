#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"
#include "Common/Utilities.hpp"
#include <cstdint>

namespace Thumb
{
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
    
    static std::string ToString(uint32_t opcode)
    {
        switch (opcode)
        {
            case ThumbOpcodes::MOV: return "MOV";
            case ThumbOpcodes::MVN: return "MVN";
            case ThumbOpcodes::AND: return "AND";
            case ThumbOpcodes::TST: return "TST";
            case ThumbOpcodes::BIC: return "BIC";
            case ThumbOpcodes::ORR: return "ORR";
            case ThumbOpcodes::EOR: return "EOR";
            case ThumbOpcodes::LSL: return "LSL";
            case ThumbOpcodes::LSR: return "LSR";
            case ThumbOpcodes::ASR: return "ASR";
            case ThumbOpcodes::ROR: return "ROR";
            case ThumbOpcodes::NOP: return "NOP";
            case ThumbOpcodes::ADD: return "ADD";
            case ThumbOpcodes::ADC: return "ADC";
            case ThumbOpcodes::SUB: return "SUB";
            case ThumbOpcodes::SBC: return "SBC";
            case ThumbOpcodes::NEG: return "NEG";
            case ThumbOpcodes::CMP: return "CMP";
            case ThumbOpcodes::CMN: return "CMN";
            case ThumbOpcodes::MUL: return "MUL";
            case ThumbOpcodes::B:   return "C";
            case ThumbOpcodes::BL:  return "BL";
            case ThumbOpcodes::BX:  return "BX";
            case ThumbOpcodes::SWI: return "SWI";
            case ThumbOpcodes::BKPT: return "BKPT";
            case ThumbOpcodes::BLX: return "BLX";
            case ThumbOpcodes::POP: return "POP";
            case ThumbOpcodes::LDR: return "LDR";
            case ThumbOpcodes::LDRB: return "LDRB";
            case ThumbOpcodes::LDRH: return "LDRH";
            case ThumbOpcodes::LDSB: return "LDSB";
            case ThumbOpcodes::LDSH: return "LDSH";
            case ThumbOpcodes::STR: return "STR";
            case ThumbOpcodes::STRB: return "STRB";
            case ThumbOpcodes::STRH: return "STRH";
            case ThumbOpcodes::PUSH: return "PUSH";
            case ThumbOpcodes::STMIA: return "STMIA";
            case ThumbOpcodes::LDMIA: return "LDMIA";
            default:
                Utilities::Assert(false, "Unknown Thumb opcode!");
                return "";
        }
    }
    
}

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction(uint16_t instruction) : Instruction(instruction) { }
    InstructionSet GetInstructionSet() override { return InstructionSet::Thumb; }
    
    virtual uint32_t GetOpcode() override = 0;
    virtual std::string ToString() override = 0;
    
    virtual bool IsImmediate() override { return false; }
};
#endif
