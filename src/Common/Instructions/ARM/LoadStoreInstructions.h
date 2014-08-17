#ifndef ARM_LDR_STR_INSTR_H
#define ARM_LDR_STR_INSTR_H

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace ARM
{
    class LoadStoreInstruction : public ARMInstruction
    {
    public:
        LoadStoreInstruction(uint32_t instruction) : ARMInstruction(instruction) { }

        uint32_t GetOpcode() const override { return MathHelper::CheckBit(_instruction, 20) ? ARMOpcodes::LDR : ARMOpcodes::STR; }
        std::string ToString() const override;

        bool IsImmediate() const override { return !MathHelper::CheckBit(_instruction, 25); }

        // The Source / Destination register depending on the instruction
        uint8_t GetRegister() const { return MathHelper::GetBits(_instruction, 12, 4); }
        uint8_t GetBaseRegister() const { return MathHelper::GetBits(_instruction, 16, 4); }
        uint8_t GetIndexRegister() const;
        
        ShiftType GetShiftType() const;
        uint8_t GetShiftImmediate() const;

        uint32_t GetImmediateOffset() const;

        bool IsPreIndexed() const { return MathHelper::CheckBit(_instruction, 24); }
        bool IsBaseAdded() const { return MathHelper::CheckBit(_instruction, 23); }
        bool IsUnsignedByte() const { return MathHelper::CheckBit(_instruction, 22); }
        bool IsUnprivileged() const;
        bool WriteBack() const;
    };
}
#endif