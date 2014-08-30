#include "LoadStoreInstructions.hpp"
#include "Common/Utilities.hpp"

#include <sstream>

bool ARM::LoadStoreInstruction::IsUnprivileged() const
{
    Utilities::Assert(!IsPreIndexed(), "Operation must be post indexed");
    return MathHelper::CheckBit(_instruction, 21);
}

bool ARM::LoadStoreInstruction::WriteBack() const
{
    // Post-Indexed load/stores always have write-back enabled
    return !IsPreIndexed() || MathHelper::CheckBit(_instruction, 21);
}

uint32_t ARM::LoadStoreInstruction::GetImmediateOffset() const
{
    Utilities::Assert(IsImmediate(), "Operation must be immediate");
    return MathHelper::GetBits(_instruction, 0, 12);
}

uint8_t ARM::LoadStoreInstruction::GetIndexRegister() const
{
    Utilities::Assert(!IsImmediate(), "Operation must not be immediate");
    return MathHelper::GetBits(_instruction, 0, 4);
}

ARM::ShiftType ARM::LoadStoreInstruction::GetShiftType() const
{
    Utilities::Assert(!IsImmediate(), "Operation must not be immediate");
    return ShiftType(MathHelper::GetBits(_instruction, 5, 2));
}

uint8_t ARM::LoadStoreInstruction::GetShiftImmediate() const
{
    Utilities::Assert(!IsImmediate(), "Operation must not be immediate");
    return MathHelper::GetBits(_instruction, 7, 5);
}

std::string ARM::LoadStoreInstruction::ToString() const
{
    std::stringstream command;
    command << ARM::ToString(GetOpcode()) << " R" << +GetRegister() << ", [R" << +GetBaseRegister();
    
    if (IsPreIndexed())
        command << ", ";
    else
        command << "], ";
    
    if (!IsBaseAdded())
        command << "-";

    if (IsImmediate())
        command << "#0x" << std::hex << +GetImmediateOffset();
    else
    {
        command << "R" << std::hex << +GetIndexRegister() << ", ";

        switch (GetShiftType())
        {
            case ShiftType::LSL:
                command << "LSL";
                break;
            case ShiftType::LSR:
                command << "LSR";
                break;
            case ShiftType::ASR:
                command << "ASR";
                break;
            case ShiftType::ROR:
                command << "ROR";
                break;
        }

        command << "#0x" << +GetShiftImmediate();
    }

    if (IsPreIndexed())
    {
        command << "]";
        if (WriteBack())
            command << "!";
    }

    return command.str();
}

uint32_t ARM::LoadStoreInstruction::GetOpcode() const
{
    if (IsMultiple())
        return MathHelper::CheckBit(_instruction, 20) ? ARMOpcodes::LDM : ARMOpcodes::STM;

    if (!IsPreIndexed() && IsUnprivileged())
    {
        // T variants
        if (IsLoad())
            return IsUnsignedByte() ? ARMOpcodes::LDRBT : ARMOpcodes::LDRT;
        else
            return IsUnsignedByte() ? ARMOpcodes::STRBT : ARMOpcodes::STRT;
    }

    if (IsLoad())
        return IsUnsignedByte() ? ARMOpcodes::LDRB : ARMOpcodes::LDR;
    else
        return IsUnsignedByte() ? ARMOpcodes::STRB : ARMOpcodes::STR;
}

uint16_t ARM::LoadStoreInstruction::GetRegistersList() const
{
    Utilities::Assert(IsMultiple(), "Instruction must be LDM or STM");
    return MathHelper::GetBits(_instruction, 0, 16);
}

uint32_t ARM::LoadStoreInstruction::GetTiming() const
{
    if (IsMultiple())
    {
        if (IsLoad())
            return 2 + MathHelper::NumberOfSetBits(GetRegistersList()) + (MathHelper::CheckBit(GetRegistersList(), 15) ? 2 : 0);
        else
            return 1 + MathHelper::NumberOfSetBits(GetRegistersList());
    }

    if (IsLoad())
        return 3 + (GetRegister() == 15 ? 2 : 0); // If Rd == PC then add 2
    else
        return 2;
}

uint32_t ARM::MiscellaneousLoadStoreInstruction::GetOpcode() const
{
    bool L = MathHelper::CheckBit(_instruction, 20);
    bool S = MathHelper::CheckBit(_instruction, 6);
    bool H = MathHelper::CheckBit(_instruction, 5);

    uint32_t opcode = 0;
    // Prior to v5TE, the bits were denoted as Load/!Store (L), Signed/!Unsigned (S) and halfword/!Byte (H) bits.
    if (!S && !H)
    {
        Utilities::Assert(false, "Error in instruction decoding");
        return 0;
    }

    if (L)
    {
        if (S)
            opcode = H ? ARMOpcodes::LDRSH : ARMOpcodes::LDRSB;
        else
            opcode = H ? ARMOpcodes::LDRH : 0;
    }
    else
    {
        if (!S)
            opcode = H ? ARMOpcodes::STRH : 0;
    }

    if (!opcode)
        Utilities::Assert(false, "Error in instruction decoding");

    return opcode;
}

std::string ARM::MiscellaneousLoadStoreInstruction::ToString() const
{
    return ARM::ToString(GetOpcode());
}

uint8_t ARM::MiscellaneousLoadStoreInstruction::GetImmediateOffset() const
{
    Utilities::Assert(IsImmediate(), "Instruction must be immediate");
    return (MathHelper::GetBits(_instruction, 8, 4) << 4) | MathHelper::GetBits(_instruction, 0, 4);
}

uint8_t ARM::MiscellaneousLoadStoreInstruction::GetRegisterOffset() const
{
    Utilities::Assert(!IsImmediate(), "Instruction must not be immediate");
    Utilities::Assert(MathHelper::GetBits(_instruction, 8, 4) == 0, "Instruction decoding error");

    return MathHelper::GetBits(_instruction, 0, 4);
}

bool ARM::MiscellaneousLoadStoreInstruction::WriteBack() const
{
    if (!IsPreIndexed() && MathHelper::CheckBit(_instruction, 21))
        Utilities::Assert(false, "Unpredictable result, W-bit must be 0 for post-indexed instructions");
    return MathHelper::CheckBit(_instruction, 21);
}

uint32_t ARM::MiscellaneousLoadStoreInstruction::GetTiming() const
{
    if (MathHelper::CheckBit(_instruction, 20))
        return 3 + (GetRegister() == 15 ? 2 : 0); // If Rd == PC then add 2
    else
        return 2;
}
