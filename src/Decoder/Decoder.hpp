#ifndef DECODER_HPP
#define DECODER_HPP

#include "Common/Instruction.hpp"

#include <cstdint>

class Decoder
{
public:
    Decoder();

    Instruction DecodeARM(uint32_t opcode);
    Instruction DecodeThumb(uint16_t opcode);
};

#endif