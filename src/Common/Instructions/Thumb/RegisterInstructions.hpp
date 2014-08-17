#ifndef THUMB_REGISTER_INSTR_H
#define THUMB_REGISTER_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    class MoveShiftedRegisterInstruction : public ThumbInstruction
    {
    public:
        MoveShiftedRegisterInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() const override;

        uint32_t GetOpcode() const override;

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 2); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetOffset() const { return MathHelper::GetBits(_instruction, 6, 4); }
    };

    class AddSubstractRegisterInstruction : public ThumbInstruction
    {
    public:
        AddSubstractRegisterInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() const override;
        uint32_t GetOpcode() const override;
        bool IsImmediate() const override { return MathHelper::GetBits(_instruction, 9, 2) >= 2; }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 2); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetOperand() const { return MathHelper::GetBits(_instruction, 6, 2); }
    };
    
    class MovCmpAddSubImmediateInstruction : public ThumbInstruction
    {
    public:
        MovCmpAddSubImmediateInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() const override;
        uint32_t GetOpcode() const override;
        bool IsImmediate() const override { return true; }

        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 8, 2); }
        uint32_t GetImmediateValue() const { return MathHelper::GetBits(_instruction, 0, 7); }
    };

    class AluInstruction : public ThumbInstruction
    {
    public:
        AluInstruction(uint16_t opcode) : ThumbInstruction(opcode) { }

        std::string ToString() const override;
        uint32_t GetOpcode() const override;

        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 2); }
    };

    class HiRegisterOperandBxInstruction : public ThumbInstruction
    {
    public:
        HiRegisterOperandBxInstruction(uint16_t instruction) : ThumbInstruction(instruction) { }

        std::string ToString() const override;
        uint32_t GetOpcode() const override;

        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 2); }
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 2); }

        // These are not used in ToString(), but they control the range of registers
        // allowed to be used as source/dest: H2 controls source, H1 controls dest.
        uint32_t GetH1() const { return (_instruction >> 7) & 0xF; }
        uint32_t GetH2() const { return (_instruction >> 6) & 0xF; }
        
        bool Link() const { return GetH1() == 1; }
    };
}

#endif // THUMB_REGISTER_INSTR_H