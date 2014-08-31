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

    class BranchInstruction : public ThumbInstruction
    {
    public:
        BranchInstruction(uint16_t op, bool isConditional) : ThumbInstruction(op),
            _conditional(isConditional) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        bool IsConditionalBranch() const { return _conditional; }

        uint32_t GetCondition() const
        {
            Utilities::Assert(!IsConditionalBranch(), "Thumb::BranchInstruction: Condition cannot be gotten if unconditional!");
            return MathHelper::GetBits(_instruction, 8, 4);
        }

        uint32_t GetImmediate() const
        {
            if (IsConditionalBranch())
                return MathHelper::GetBits(_instruction, 0, 8);
            return MathHelper::GetBits(_instruction, 0, 10);
        }

        int32_t GetBranchOffset() const
        {
            return int32_t(GetImmediate() << 1);
        }

    private:
        bool _conditional;
    };

    class LongBranchLinkInstruction : public ThumbInstruction
    {
    public:
        LongBranchLinkInstruction(uint16_t opcode) : ThumbInstruction(opcode) { }

        bool IsTriggeringSubroutineCall() const
        {
            // To allow for a reasonably large offset to the target subroutine,
            // the BL (or BLX) instruction is automatically translated by the
            // assembler into a sequence of two 16-bit Thumb instructions:
            // - The first Thumb instruction has H == 0b10 and supplies the high part
            //   of the branch offset. This instruction sets up for the subroutine
            //   call (and is shared between the BL and BLX forms).
            // - The second Thumb instruction has H == 0b11 (for BL) (or H == 0b01 (for BLX)).
            //   It supplies the low part of the branch offset and causes the subroutine
            //   call to take place.
            // H can only be 0b01 (BLX) on ARMv5T and above. So this is always a BL.
            return MathHelper::CheckBit(_instruction, 11);
        }

        uint32_t GetOpcode() const override { return ThumbOpcodes::BL; }
        std::string ToString() const override;

        int32_t GetOffset() const
        {
            return MathHelper::GetBits(_instruction, 0, 11);
        }
    };
}

#endif // THUMB_BX_INSTR_H
