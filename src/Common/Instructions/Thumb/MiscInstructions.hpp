#ifndef THUMB_MISC_INSTR_H
#define THUMB_MISC_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    class StackOperation : public ThumbInstruction
    {
    public:
        StackOperation(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsPopOperand() const { return MathHelper::CheckBit(_instruction, 11); }
        uint32_t GetRegisterMask() const { return MathHelper::GetBits(_instruction, 0, 9); }
    };
}

#endif // THUMB_MISC_INSTR_H
