#ifndef THUMB_REGISTER_INSTR_H
#define THUMB_REGISTER_INSTR_H

#include "Common/Instructions/ARMInstruction.hpp"

namespace Thumb
{
    class MoveShiftedRegisterInstruction : public ThumbInstruction
    {
    public:
        MoveShiftedRegisterInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() override;

        uint32_t GetOpcode() override;

        uint32_t GetDestinationRegister() { return MathHelper::GetBits(_instruction, 0, 2); }
        uint32_t GetSourceRegister() { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetOffset() { return MathHelper::GetBits(_instruction, 6, 4); }
    };

    class AddSubstractRegisterInstruction : public ThumbInstruction
    {
    public:
        AddSubstractRegisterInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() override;

        uint32_t GetOpcode() override;

        uint32_t GetDestinationRegister() { return MathHelper::GetBits(_instruction, 0, 2); }
        uint32_t GetSourceRegister() { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetOperand() { return MathHelper::GetBits(_instruction, 6, 2); }
        bool IsImmediate() { return MathHelper::GetBits(_instruction, 9, 2) > 2; }
    };
}

#endif // THUMB_REGISTER_INSTR_H