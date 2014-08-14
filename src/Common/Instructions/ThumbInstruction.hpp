#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction(uint16_t instruction) : Instruction(instruction) { }
    InstructionSet GetInstructionSet() override { return InstructionSet::Thumb; }
};
#endif