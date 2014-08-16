#include "Common/Utilities.hpp"
#include "DataProcessingInstructions.hpp"
#include <sstream>

uint8_t ARM::DataProcessingInstruction::GetSecondOperand()
{
    return IsImmediate() ? _instruction & 0xFF : _instruction & 0xF;
}

uint8_t ARM::DataProcessingInstruction::GetShiftImmediate()
{
    Utilities::Assert(IsImmediate(), "Instruction must be immediate");
    return ((_instruction >> 8) & 0xF) << 2;
}

ARM::ShiftType ARM::DataProcessingInstruction::GetShiftType()
{
    if (IsImmediate())
        return ShiftType::ROR;

    return ShiftType((_instruction >> 5) & 3);
}

bool ARM::DataProcessingInstruction::ShiftByRegister()
{
    Utilities::Assert(!IsImmediate(), "Instruction must not be immediate");
    return (_instruction >> 4) & 1;
}

uint8_t ARM::DataProcessingInstruction::GetShiftRegisterOrImmediate()
{
    Utilities::Assert(!IsImmediate(), "Instruction must not be immediate");

    if (ShiftByRegister())
        return (_instruction >> 8) & 0xF; // 4 bits

    return (_instruction >> 7) & 0x1F; // 5 bits
}

uint32_t ARM::DataProcessingInstruction::GetOpcode()
{
    uint8_t op = (_instruction >> 21) & 0xF;

    switch (op)
    {
        case 0:
            return ARMOpcodes::AND;
        case 1:
            return ARMOpcodes::EOR;
        case 2:
            return ARMOpcodes::SUB;
        case 3:
            return ARMOpcodes::RSB;
        case 4:
            return ARMOpcodes::ADD;
        case 5:
            return ARMOpcodes::ADC;
        case 6:
            return ARMOpcodes::SBC;
        case 7:
            return ARMOpcodes::RSC;
        case 8:
            return ARMOpcodes::TST;
        case 9:
            return ARMOpcodes::TEQ;
        case 0xA:
            return ARMOpcodes::CMP;
        case 0xB:
            return ARMOpcodes::CMN;
        case 0xC:
            return ARMOpcodes::ORR;
        case 0xD:
            return ARMOpcodes::MOV;
        case 0xE:
            return ARMOpcodes::BIC;
        case 0xF:
            return ARMOpcodes::MVN;
        default:
            Utilities::Assert(false, "Invalid data processing instruction opcode");
            return ARMOpcodes(op);
    }
}

std::string ARM::DataProcessingInstruction::ToString()
{
    std::stringstream command;

    switch (GetOpcode())
    {
        case ARMOpcodes::AND:
            command << "AND ";
            break;
        case ARMOpcodes::EOR:
            command << "EOR ";
            break;
        case ARMOpcodes::SUB:
            command << "SUB ";
            break;
        case ARMOpcodes::RSB:
            command << "RSB ";
            break;
        case ARMOpcodes::ADD:
            command << "ADD ";
            break;
        case ARMOpcodes::ADC:
            command << "ADC ";
            break;
        case ARMOpcodes::SBC:
            command << "SBC ";
            break;
        case ARMOpcodes::RSC:
            command << "RSC ";
            break;
        case ARMOpcodes::TST:
            command << "TST ";
            break;
        case ARMOpcodes::TEQ:
            command << "TEQ ";
            break;
        case ARMOpcodes::CMP:
            command << "CMP ";
            break;
        case ARMOpcodes::CMN:
            command << "CMN ";
            break;
        case ARMOpcodes::ORR:
            command << "ORR ";
            break;
        case ARMOpcodes::MOV:
            command << "MOV ";
            break;
        case ARMOpcodes::BIC:
            command << "BIC ";
            break;
        case ARMOpcodes::MVN:
            command << "MVN ";
            break;
    }

    if (GetFirstOperand())
        command << "R" << GetFirstOperand() << ", ";

    if (GetDestinationRegister())
        command << "R" << GetDestinationRegister() << ", ";

    if (IsImmediate())
        command << "#" << GetSecondOperand();
    return command.str();
}

