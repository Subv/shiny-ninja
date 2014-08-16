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
    command << ARM::ToString(GetOpcode()) << " ";

    if (GetFirstOperand())
        command << "R" << GetFirstOperand() << ", ";

    if (GetDestinationRegister())
        command << "R" << GetDestinationRegister() << ", ";

    if (IsImmediate())
        command << "#" << GetSecondOperand();
    return command.str();
}

