#ifndef THUMB_DATAPROC_INSTR_H
#define THUMB_DATAPROC_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
    //  0  0  0    Op        Offset5       Rs       Rd
    class ImmediateShiftInstruction : public ThumbInstruction
    {
    public:
        ImmediateShiftInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
        uint32_t GetOffset() const { return MathHelper::GetBits(_instruction, 6, 5); }
    };

    // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
    //  0  0  0  1  1  I Op       Rn       Rs       Rd
    class AddSubInstruction : public ThumbInstruction
    {
    public:
        AddSubInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        bool IsImmediate() const override { return MathHelper::CheckBit(_instruction, 10) == 1; }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
        uint32_t GetThirdOperand() const { return MathHelper::GetBits(_instruction, 6, 3); }
    };

    class AddSubCmpMovImmInstruction : public ThumbInstruction
    {
    public:
        AddSubCmpMovImmInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        bool IsImmediate() const override { return true; }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 8, 3); }
        uint32_t GetImmediate() const { return MathHelper::GetBits(_instruction, 0, 8); }
    };

    class DataProcessingInstruction : public ThumbInstruction
    {
    public:
        DataProcessingInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
    };

    class SpecialDataProcessingInstruction : public ThumbInstruction
    {
    public:
        SpecialDataProcessingInstruction(uint16_t op) : ThumbInstruction(op) { }

        uint32_t GetOpcode() const override;
        std::string ToString() const override;

        uint32_t GetDestinationRegister() const
        {
            if (HiOperandFlagDestination())
                return 8 + MathHelper::GetBits(_instruction, 0, 3);
            return MathHelper::GetBits(_instruction, 0, 3);
        }

        uint32_t GetFirstDataRegister() const
        {
            if (HiOperandFlagSource())
                return 8 + MathHelper::GetBits(_instruction, 3, 3);
            return MathHelper::GetBits(_instruction, 3, 3);
        }

        bool HiOperandFlagSource() const { return MathHelper::CheckBit(_instruction, 6); }
        bool HiOperandFlagDestination() const { return MathHelper::CheckBit(_instruction, 7); }
    };
}

#endif // THUMB_DATAPROC_INSTR_H
