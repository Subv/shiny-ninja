#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Common/Instructions/Instruction.hpp"

#include <memory>

class Interpreter
{
public:
    Interpreter() { }

    void RunInstruction(std::shared_ptr<Instruction> instruction);
};
#endif