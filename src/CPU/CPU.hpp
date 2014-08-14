#ifndef CPU_HPP
#define CPU_HPP

#include "Decoder/Decoder.hpp"
#include "Interpreter/Interpreter.hpp"

#include <cstdio>

struct GBAHeader;

enum class CPUMode
{
    Interpreter,
    JIT
};

enum class CPURunState
{
    Running,
    Stepping,
    Stopped
};

#pragma pack(push, 1)
union CPSR
{
    struct
    {
        uint8_t M : 5;
        uint8_t T : 1;
        uint8_t F : 1;
        uint8_t I : 1;
        uint8_t A : 1;
        uint8_t E : 1;
        uint8_t IT1 : 6;
        uint8_t GE : 4;
        uint8_t DNM : 4;
        uint8_t J : 1;
        uint8_t IT2 : 2;
        uint8_t Q : 1;
        uint8_t V : 1;
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t N : 1;
    } Flags;

    uint32_t Full;
};
#pragma pack(pop)

struct CPUState
{
    uint32_t Registers[16];
    CPSR CPSR;
};

class CPU final : public std::enable_shared_from_this<CPU>
{
public:
    CPU(CPUMode mode);

    void LoadROM(GBAHeader& header, FILE* rom);
    void Reset();
    void Run();

    InstructionSet GetCurrentInstructionSet() { return InstructionSet(_state.CPSR.Flags.I); }

private:
    CPUMode _mode;
    CPURunState _runState;
    CPUState _state;
    std::unique_ptr<Interpreter> _interpreter;
    std::unique_ptr<Decoder> _decoder;
};

// Define shorthands for the most commonly used registers
#define PC _state.Registers[15]
#define LR _state.Registers[14]
#define SP _state.Registers[13]:

#endif