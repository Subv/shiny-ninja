#include "RegisterInstructions.hpp"

#include <cstdint>

std::string Thumb::MoveShiftedRegisterInstruction::ToString() override
{
    std::ostringstream stream;
    switch (GetOpcode())
    {
        case ThumbOpcodes::LSL:
            stream << "LSL R";
            break;
        case ThumbOpcodes::LSR:
            stream << "LSR R";
            break;
        case ThumbOpcodes::ASR:
        default: // Should never happen
            stream << "ASR R";
            break;
    }

    stream << GetDestinationRegister() << ", R" << GetSourceRegister() << " #" << GetOffset();
    return stream.str();
}

uint32_t Thumb::MoveShiftedRegisterInstruction::GetOpcode() override
{
    switch (MathHelper::GetBits(_instruction, 11, 2))
    {
        case 0x0: // LSL
            return ThumbOpcodes::LSL;
        case 0x1: // LSR
            return ThumbOpcodes::LSR;
        case 0x2: // ASR
        default: // Should never happen
            return ThumbOpcodes::ASR;
    }
}

std::string Thumb::AddSubstractRegisterInstruction::ToString() override
{
    /*
    10-9   Opcode (0-3)
               0: ADD Rd,Rs,Rn   ;add register        Rd=Rs+Rn
               1: SUB Rd,Rs,Rn   ;subtract register   Rd=Rs-Rn
               2: ADD Rd,Rs,#nn  ;add immediate       Rd=Rs+nn
               3: SUB Rd,Rs,#nn  ;subtract immediate  Rd=Rs-nn
             Pseudo/alias opcode with Imm=0:                              //! TODO No clue what this means!
               2: MOV Rd,Rs      ;move (affects cpsr) Rd=Rs+0
    */
    std::ostringstream stream;
    switch (GetOpcode())
    {
        case ThumbOpcodes::ADD:
            stream << "ADD ";
            break;
        case ThumbOpcodes::SUB;
            stream << "SUB ";
    }

    stream << "R" << GetDestinationRegister() << ", R" << GetSourceRegister() << (!IsImmediate() ? ", R" : ", #") << GetOperand();
    return stream.str();
}

uint32_t  Thumb::AddSubstractRegisterInstruction::GetOpcode() override
{
    switch (GetOpcode())
    {
        case 0x0:
        case 0x2:
            return ThumbOpcodes::ADD;
        case 0x1:
        case 0x3:
            return ThumbOpcodes::SUB;
    }
}
