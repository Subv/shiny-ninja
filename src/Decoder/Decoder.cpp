#include "Decoder.hpp"
#include "Common/Instructions/ARM/BranchInstruction.hpp"

#include "Common/MathHelper.hpp"

using std::shared_ptr;

Decoder::Decoder()
{
    
}

shared_ptr<Instruction> Decoder::DecodeARM(uint32_t opcode)
{
    std::terminate();
}

shared_ptr<Instruction> Decoder::DecodeThumb(uint16_t opcode)
{
    std::terminate();
}