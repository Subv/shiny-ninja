#include "LoadStoreInstructions.h"
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


