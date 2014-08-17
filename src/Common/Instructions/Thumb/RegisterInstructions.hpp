#ifndef THUMB_REGISTER_INSTR_H
#define THUMB_REGISTER_INSTR_H

#include "Common/Instructions/ThumbInstruction.hpp"
#include "Common/MathHelper.hpp"

namespace Thumb
{
    class ImmediateShiftInstruction : public ThumbInstruction
    {
    public:
        ImmediateShiftInstruction(uint16_t op) : ThumbInstruction(op) { }
        
        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        bool IsImmediate() const override { return true; }
        
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
        uint32_t GetImmediateValue() const { return MathHelper::GetBits(_instruction, 6, 5); }
    };
    
    class AddSubRegisterInstruction : public ThumbInstruction
    {
    public:
        AddSubRegisterInstruction(uint16_t op) : ThumbInstruction(op) { }
        
        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 0, 3); }
        uint32_t GetSourceRegister() const { return MathHelper::GetBits(_instruction, 3, 3); }
        bool IsImmediate() const override { return MathHelper::CheckBit(_instruction, 10) == 1; }
        uint32_t GetThirdOperand() const { return MathHelper::GetBits(_instruction, 6, 3); }
    };
    
    class MovCmpAddSubImmediateInstruction : public ThumbInstruction
    {
    public:
        MovCmpAddSubImmediateInstruction(uint16_t op) : ThumbInstruction(op) { }
        
        uint32_t GetOpcode() const override;
        std::string ToString() const override;
        bool IsImmediate() const override { return true; }
        
        uint32_t GetDestinationRegister() const { return MathHelper::GetBits(_instruction, 8, 3); }
        uint32_t GetImmediateValue() const { return MathHelper::GetBits(_instruction, 0, 8); }
    };
}

#endif // THUMB_REGISTER_INSTR_H