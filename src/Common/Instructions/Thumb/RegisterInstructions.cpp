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

uint32_t Thumb::AluInstruction::GetOpcode()
{
    switch (MathHelper::GetBits(_instruction, 6, 3))
    {
        case 0: return ThumbOpcodes::AND;
        case 1: return ThumbOpcodes::EOR;
        case 2: return ThumbOpcodes::LSL;
        case 3: return ThumbOpcodes::LSR;
        case 4: return ThumbOpcodes::ASR;
        case 5: return ThumbOpcodes::ADC;
        case 6: return ThumbOpcodes::SBC;
        case 7: return ThumbOpcodes::ROR;
        case 8: return ThumbOpcodes::TST;
        case 9: return ThumbOpcodes::NEG;
        case 10: return ThumbOpcodes::CMP;
        case 11: return ThumbOpcodes::CMN;
        case 12: return ThumbOpcodes::ORR;
        case 13: return ThumbOpcodes::MUL;
        case 14: return ThumbOpcodes::BIC;
        case 15: return ThumbOpcodes::MVN;
        default:
            Utilities::Assert(false, "Thumb::AluInstruction has invalid opcode");
            break;
    }
    return 0;
}

std::string Thumb::AluInstruction::ToString()
{
    std::stringstream stream;
    stream << Thumb::ToString(GetOpcode()) << " ";
    stream << GetDestinationRegister() << ", " << GetSourceRegister();
    return stream.str();
}

uint32_t Thumb::HiRegisterOperandBxInstruction::GetOpcode()
{
    switch (MathHelper::GetBits(_instruction, 8, 2))
    {
        case 0: return ThumbOpcodes::ADD;
        case 1: return ThumbOpcodes::CMP;
        case 2: return ThumbOpcodes::MOV;
        case 3: return ThumbOpcodes::BX;
        default:
            Utilities::Assert(false, "Thumb::HiRegisterOperandBxInstruction has invalid opcode");
            break;
    }
}

std::string Thumb::HiRegisterOperandBxInstruction::ToString()
{
    if (GetOpcode() == ThumbOpcodes::MOV && GetDestinationRegister() == 8 && GetSourceRegister() == 8)
        return "NOP";
    
    std::stringstream stream;
    
    stream << Thumb::ToString(GetOpcode()) << " " << GetDestinationRegister();
    if (GetOpcode() != ThumbOpcodes::BX)
        stream << ", " << GetSourceRegister();
    return stream.str();
}
