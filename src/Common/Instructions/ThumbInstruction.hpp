#ifndef THUMBINSTRUCTION_HPP
#define THUMBINSTRUCTION_HPP

#include "Instruction.hpp"

class ThumbInstruction : public Instruction
{
public:
    ThumbInstruction() : Instruction() { }
    InstructionSet GetInstructionSet() override { return InstructionSet::Thumb; }
};
#endif