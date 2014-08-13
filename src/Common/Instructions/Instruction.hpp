#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <cstdint>

enum class InstructionSet
{
    ARM,
    Thumb
};

class Instruction
{
public:
    Instruction() : _instruction(0) { }
    virtual ~Instruction() { }

    virtual std::string ToString() = 0;
    virtual InstructionSet GetInstructionSet() = 0;

    virtual uint32_t GetOpcode() = 0;

protected:
    uint32_t _instruction;
};
#endif