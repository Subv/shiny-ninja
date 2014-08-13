#include "ARMInstruction.hpp"

InstructionCondition ARMInstruction::GetCondition()
{
    // The condition is the last 4 bits of the instruction
    uint8_t cond = _instruction >> 28;
    return InstructionCondition(cond);
}