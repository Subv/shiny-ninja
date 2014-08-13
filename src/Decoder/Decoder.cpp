#include "Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstructions.hpp"
#include "Common/Instructions/ARM/DataProcessingInstructions.hpp"

#include "Common/MathHelper.hpp"

using std::shared_ptr;

Decoder::Decoder()
{
    
}

shared_ptr<Instruction> Decoder::DecodeARM(uint32_t opcode)
{
    // Check for BLX and BX
    if (MathHelper::CheckBits(opcode, 8, 20, 0x12FFF))
        return shared_ptr<Instruction>(new ARM::BranchLinkExchangeRegisterInstruction(opcode));

    // Now check for B, BL and BLX_IMM
    if (MathHelper::CheckBits(opcode, 25, 3, 0x5))
    {
        if (MathHelper::CheckBits(opcode, 27, 4, 0xF))
            return shared_ptr<Instruction>(new ARM::BranchLinkExchangeImmediateInstruction(opcode));

        return shared_ptr<Instruction>(new ARM::BranchInstruction(opcode));
    }

    // Check for data processing instructions
    if (MathHelper::CheckBits(opcode, 26, 2, 0))
        return shared_ptr<Instruction>(new ARM::DataProcessingInstruction(opcode));

    return shared_ptr<Instruction>(nullptr);
}

shared_ptr<Instruction> Decoder::DecodeThumb(uint16_t opcode)
{
    return shared_ptr<Instruction>(nullptr);
}