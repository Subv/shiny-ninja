#include "DataProcessingInstructions.hpp"

#include "Common/Utilities.hpp"
#include "Common/MathHelper.hpp"

#include <sstream>

uint8_t ARM::DataProcessingInstruction::GetSecondOperand() const
{
    return IsImmediate() ? _instruction & 0xFF : _instruction & 0xF;
}

uint8_t ARM::DataProcessingInstruction::GetShiftImmediate() const
{
    Utilities::Assert(IsImmediate(), "Instruction must be immediate");
    return (_instruction >> 8) & 0xF;
}

ARM::ShiftType ARM::DataProcessingInstruction::GetShiftType() const
{
    if (IsImmediate())
        return ShiftType::ROR;

    return ShiftType((_instruction >> 5) & 3);
}

bool ARM::DataProcessingInstruction::ShiftByRegister() const
{
    Utilities::Assert(!IsImmediate(), "Instruction must not be immediate");
    return (_instruction >> 4) & 1;
}

uint8_t ARM::DataProcessingInstruction::GetShiftRegisterOrImmediate() const
{
    Utilities::Assert(!IsImmediate(), "Instruction must not be immediate");

    if (ShiftByRegister())
        return (_instruction >> 8) & 0xF; // 4 bits

    return (_instruction >> 7) & 0x1F; // 5 bits
}

uint32_t ARM::DataProcessingInstruction::GetOpcode() const
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

std::string ARM::DataProcessingInstruction::ToString() const
{
    std::stringstream command;
    command << ARM::ToString(GetOpcode()) << " ";

    if (HasDestinationRegister())
        command << "R" << +GetDestinationRegister() << ", ";

    if (HasFirstOperand())
        command << "R" << +GetFirstOperand() << ", ";

    if (IsImmediate())
        command << "#" << std::hex << +GetShiftedSecondOperandImmediate();

    // TODO: Add immediate shifts and register as second operand

    return command.str();
}

bool ARM::DataProcessingInstruction::HasFirstOperand() const
{
    switch (GetOpcode())
    {
        case ARMOpcodes::MOV:
        case ARMOpcodes::MVN:
            return false;
        default:
            break;
    }

    return true;
}

bool ARM::DataProcessingInstruction::HasDestinationRegister() const
{
    switch (GetOpcode())
    {
        case ARMOpcodes::CMP:
        case ARMOpcodes::CMN:
        case ARMOpcodes::TST:
        case ARMOpcodes::TEQ:
            return false;
        default:
            break;
    }

    return true;
}

uint32_t ARM::DataProcessingInstruction::GetShiftedSecondOperandImmediate() const
{
    Utilities::Assert(IsImmediate(), "Opcode must be immediate");

    // Performs the ROR shift as defined in the opcode encoding.
    return MathHelper::RotateRight(uint32_t(GetSecondOperand()), GetShiftImmediate() << 1);
}

bool ARM::DataProcessingInstruction::AffectsOverflow() const
{
    switch (GetOpcode())
    {
        case ARMOpcodes::AND:
        case ARMOpcodes::EOR:
        case ARMOpcodes::TST:
        case ARMOpcodes::TEQ:
        case ARMOpcodes::ORR:
        case ARMOpcodes::MOV:
        case ARMOpcodes::BIC:
        case ARMOpcodes::MVN:
            return false;
    }

    return true;
}
