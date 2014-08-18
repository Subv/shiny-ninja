#include "LoadStoreInstructions.hpp"
#include "Common/Utilities.hpp"

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
    return ARM::ToString(GetOpcode());
}

uint32_t ARM::LoadStoreInstruction::GetOpcode() const
{
    if (IsMultiple())
        return MathHelper::CheckBit(_instruction, 20) ? ARMOpcodes::LDM : ARMOpcodes::STM;

    if (!IsPreIndexed() && MathHelper::CheckBit(_instruction, 21))
    {
        // T variants
        if (MathHelper::CheckBit(_instruction, 20))
            return IsUnsignedByte() ? ARMOpcodes::LDRBT : ARMOpcodes::LDRT;
        else
            return IsUnsignedByte() ? ARMOpcodes::STRBT : ARMOpcodes::STRT;
    }

    if (MathHelper::CheckBit(_instruction, 20))
        return IsUnsignedByte() ? ARMOpcodes::LDRB : ARMOpcodes::LDR;
    else
        return IsUnsignedByte() ? ARMOpcodes::STRB : ARMOpcodes::STR;
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
