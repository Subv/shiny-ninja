#ifndef CPU_HPP
#define CPU_HPP

#include "Decoder/Decoder.hpp"
#include "Interpreter/Interpreter.hpp"

#include <memory>

enum class CPUMode
{
    Interpreter,
    JIT
};

enum class CPUState
{
    Running,
    Stepping,
    Stopped
};

class CPU
{
public:
    CPU(CPUMode mode);

    void Reset();
    void Run();

private:
    CPUMode _mode;
    CPUState _state;
    std::unique_ptr<Interpreter> _interpreter;
    std::unique_ptr<Decoder> _decoder;
};
#endif