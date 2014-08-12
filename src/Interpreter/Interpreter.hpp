#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Common/Instruction.hpp"

class Interpreter
{
public:
    Interpreter() { }

    void RunInstruction(Instruction instruction);
};
#endif