#include "ARMInstruction.hpp"

InstructionCondition ARMInstruction::GetCondition()
{
    // The condition is the last 4 bits of the instruction
    uint8_t cond = _instruction >> 28;
    return InstructionCondition(cond);
}

std::string ARM::ToString(uint32_t opcode)
{
    switch (opcode)
    {
        case ADC: return "ADC";
        case ADD: return "ADD";
        case AND: return "AND";
        case B:   return "B";
        case BL:  return "BL";
        case BIC: return "BIC";
        case BLX: return "BLX";
        case BX:  return "BX";
        case CDP: return "CDP";
        case CMN: return "CMN";
        case CMP: return "CMP";
        case EOR: return "EOR";
        case LDC: return "LDC";
        case LDM: return "LDM"; // TODO: Multiple forms
        case LDR: return "LDR";
        case LDRB: return "LDRB";
        case LDRBT: return "LDRBT";
        case LDRH: return "LDRH";
        case LDRSB: return "LDRSB";
        case LDRSH: return "LDRSH";
        case LDRT: return "LDRT";
        case MCR: return "MCR";
        case MLA: return "MLA";
        case MOV: return "MOV";
        case MRC: return "MRC";
        case MRS: return "MRS";
        case MSR: return "MSR";
        case MUL: return "MUL";
        case MVN: return "MVN";
        case ORR: return "ORR";
        case RSB: return "RSB";
        case RSC: return "RSC";
        case SBC: return "SBC";
        case SMLAL: return "SMLAL";
        case SMULL: return "SMULL";
        case STC: return "STC";
        case STM: return "STM"; // TODO: Multiple forms
        case STR: return "STR";
        case STRB: return "STRB";
        case STRBT: return "STRBT";
        case STRH: return "STRH";
        case STRT: return "STRT";
        case SUB: return "SUB";
        case SWI: return "SWI";
        case SWP: return "SWP";
        case SWPB: return "SWPB";
        case TEQ: return "TEQ";
        case TST: return "TST";
        case UMLAL: return "UMLAL";
        case UMULL: return "UMULL";
    }
}