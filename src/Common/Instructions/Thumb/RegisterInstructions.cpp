#include "RegisterInstructions.hpp"

#include "Common/Utilities.hpp"

#include <cstdint>
#include <sstream>

std::string Thumb::MoveShiftedRegisterInstruction::ToString()
{
    std::ostringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister();
    stream << ", R" << GetSourceRegister() << " #" << GetOffset();
    return stream.str();
}

uint32_t Thumb::MoveShiftedRegisterInstruction::GetOpcode()
{
    switch (MathHelper::GetBits(_instruction, 11, 2))
    {
        case 0x0: // LSL
            return ThumbOpcodes::LSL;
        case 0x1: // LSR
            return ThumbOpcodes::LSR;
        case 0x2: // ASR
            return ThumbOpcodes::ASR;
        default:
            Utilities::Assert(false, "Thumb::MoveShiftedRegisterInstruction has invalid opcode");
            return 0;
    }
}

std::string Thumb::AddSubstractRegisterInstruction::ToString()
{
    /*
    10-9   Opcode (0-3)
               0: ADD Rd,Rs,Rn   ;add register        Rd=Rs+Rn
               1: SUB Rd,Rs,Rn   ;subtract register   Rd=Rs-Rn
               2: ADD Rd,Rs,#nn  ;add immediate       Rd=Rs+nn
               3: SUB Rd,Rs,#nn  ;subtract immediate  Rd=Rs-nn
             Pseudo/alias opcode with Imm=0:
               2: MOV Rd,Rs      ;move (affects cpsr) Rd=Rs+0
    */
    std::ostringstream stream;
    switch (GetOpcode())
    {
        case ThumbOpcodes::ADD:
            stream << "ADD ";
            break;
        case ThumbOpcodes::SUB:
            stream << ((IsImmediate() && GetOperand() == 0) ? "MOV " : "SUB ");
            break;
    }

    stream << "R" << GetDestinationRegister() << ", R" << GetSourceRegister();
    if (IsImmediate() && GetOperand() == 0)
        return stream.str();
    stream << (!IsImmediate() ? ", R" : ", #") << GetOperand();
    return stream.str();
}

uint32_t Thumb::AddSubstractRegisterInstruction::GetOpcode()
{
    switch (GetOpcode())
    {
        case 0x0:
        case 0x2:
            return ThumbOpcodes::ADD;
        case 0x1:
        case 0x3:
            return ThumbOpcodes::SUB;
        default:
            Utilities::Assert(false, "Thumb::AddSubstractRegisterInstruction has invalid opcode");
            break;
    }

    return 0;
}

uint32_t Thumb::MovCmpAddSubImmediateInstruction::GetOpcode()
{
    switch (MathHelper::GetBits(_instruction, 11, 2))
    {
        case 0: return ThumbOpcodes::MOV;
        case 1: return ThumbOpcodes::CMP;
        case 2: return ThumbOpcodes::ADD;
        case 3: return ThumbOpcodes::SUB;
        default:
            Utilities::Assert(false, "Thumb::MovCmpAddSubImmediateInstruction has invalid opcode");
            break;
    }
    return 0;
}

std::string Thumb::MovCmpAddSubImmediateInstruction::ToString()
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " R" << GetDestinationRegister() << ", #" << GetImmediateValue();
    return stream.str();
}
