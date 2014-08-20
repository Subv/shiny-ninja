#ifndef THUMB_BX_INSTR_H
#define THUMB_BX_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    class BranchExchangeInstruction : public ThumbInstruction
    {
    public:
        BranchExchangeInstruction(uint16_t op) : ThumbInstruction(op)
        {
        }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        // Prior to ARMv5T, 7th bit set to 1 is UNPREDICTABLE
        // bool Link() const { return MathHelper::CheckBit(_instruction, 7); }

        uint32_t GetTargetAddressRegister() const { return MathHelper::GetBits(_instruction, 3, 4); }
    };
}

#endif // THUMB_BX_INSTR_H
