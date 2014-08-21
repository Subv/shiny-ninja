#include "ThumbInstruction.hpp"

std::string Thumb::ToString(uint32_t opcode)
{
    switch (opcode)
    {
        case ThumbOpcodes::ADC:
            return "ADC";
        case ThumbOpcodes::ADD_1:
        case ThumbOpcodes::ADD_2:
        case ThumbOpcodes::ADD_3:
        case ThumbOpcodes::ADD_4:
        case ThumbOpcodes::ADD_SP:
        case ThumbOpcodes::ADD_PC:
        case ThumbOpcodes::ADD_7:
            return "ADD";
        case ThumbOpcodes::AND:
            return "AND";
        case ThumbOpcodes::ASR_1:
        case ThumbOpcodes::ASR_2:
            return "ASR";
        case ThumbOpcodes::B_CONDITIONAL:
        case ThumbOpcodes::B_UNCONDITIONAL:
            return "B";
        case ThumbOpcodes::BIC:
            return "BIC";
        case ThumbOpcodes::BKPT:
            return "BKPT";
        case ThumbOpcodes::BL:
            return "BL";
        case ThumbOpcodes::BLX_1:
        case ThumbOpcodes::BLX_2:
            return "BLX";
        case ThumbOpcodes::BX:
            return "BX";
        case ThumbOpcodes::CMN:
            return "CMN";
        case ThumbOpcodes::CMP_1:
        case ThumbOpcodes::CMP_2:
        case ThumbOpcodes::CMP_3:
            return "CMP";
        case ThumbOpcodes::CPS:
            return "CPS";
        case ThumbOpcodes::CPY:
            return "CPY";
        case ThumbOpcodes::EOR:
            return "EOR";
        case ThumbOpcodes::LDMIA:
            return "LDMIA";
        case ThumbOpcodes::LDR_1:
        case ThumbOpcodes::LDR_2:
        case ThumbOpcodes::LDR_3:
        case ThumbOpcodes::LDR_4:
            return "LDR";
        case ThumbOpcodes::LDRB_1:
        case ThumbOpcodes::LDRB_2:
            return "LDRB";
        case ThumbOpcodes::LDRH_1:
        case ThumbOpcodes::LDRH_2:
            return "LDRH";
        case ThumbOpcodes::LDRSB:
            return "LDRSB";
        case ThumbOpcodes::LDRSH:
            return "LDRSH";
        case ThumbOpcodes::LSL_1:
        case ThumbOpcodes::LSL_2:
            return "LSL";
        case ThumbOpcodes::LSR_1:
        case ThumbOpcodes::LSR_2:
            return "LSR";
        case ThumbOpcodes::MOV_1:
        case ThumbOpcodes::MOV_2:
        case ThumbOpcodes::MOV_3:
            return "MOV";
        case ThumbOpcodes::MUL:
            return "MUL";
        case ThumbOpcodes::MVN:
            return "MVN";
        case ThumbOpcodes::NEG:
            return "NEG";
        case ThumbOpcodes::ORR:
            return "ORR";
        case ThumbOpcodes::POP:
            return "POP";
        case ThumbOpcodes::PUSH:
            return "PUSH";
        case ThumbOpcodes::REV:
            return "REV";
        case ThumbOpcodes::REV16:
            return "REV16";
        case ThumbOpcodes::REVSH:
            return "REVSH";
        case ThumbOpcodes::ROR:
            return "ROR";
        case ThumbOpcodes::SBC:
            return "SBC";
        case ThumbOpcodes::SETEND:
            return "SETEND";
        case ThumbOpcodes::STMIA:
            return "STMIA";
        case ThumbOpcodes::STR_1:
        case ThumbOpcodes::STR_2:
        case ThumbOpcodes::STR_3:
            return "STR";
        case ThumbOpcodes::STRB_1:
        case ThumbOpcodes::STRB_2:
            return "STRB";
        case ThumbOpcodes::STRH_1:
        case ThumbOpcodes::STRH_2:
            return "STRH";
        case ThumbOpcodes::SUB_1:
        case ThumbOpcodes::SUB_2:
        case ThumbOpcodes::SUB_3:
        case ThumbOpcodes::SUB_4:
            return "SUB";
        case ThumbOpcodes::SWI:
            return "SWI";
        case ThumbOpcodes::SXTB:
            return "SXTB";
        case ThumbOpcodes::SXTH:
            return "SXTH";
        case ThumbOpcodes::TST:
            return "TST";
        case ThumbOpcodes::UXTB:
            return "UXTB";
        case ThumbOpcodes::UXTH:
            return "UXTH";
        default:
            Utilities::Assert(false, "Unknown Thumb opcode!");
            return "";
    }
}
