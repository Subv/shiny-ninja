#ifndef DECODER_HPP
#define DECODER_HPP

#include "Common/Instructions/Instruction.hpp"

#include <memory>
#include <cstdint>

class Decoder final
{
public:
    Decoder();

    std::shared_ptr<Instruction> DecodeARM(uint32_t opcode);
    std::shared_ptr<Instruction> DecodeThumb(uint16_t opcode);
};

#endif