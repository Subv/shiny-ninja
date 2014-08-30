#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <cstdint>

enum class InstructionSet
{
    ARM  = 0,
    Thumb
};

class Instruction
{
public:
    Instruction(uint32_t instruction) : _instruction(instruction) { }
    virtual ~Instruction() { }

    virtual uint32_t GetTiming() const { return 1; }

    virtual std::string ToString() const = 0;
    virtual InstructionSet GetInstructionSet() const = 0;

    virtual uint32_t GetOpcode() const = 0;
    virtual bool IsImmediate() const = 0;

protected:
    uint32_t _instruction;
};

#endif
