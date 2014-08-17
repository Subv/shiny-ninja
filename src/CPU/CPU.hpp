#ifndef CPU_HPP
#define CPU_HPP

#include "Decoder/Decoder.hpp"
#include "Interpreter/Interpreter.hpp"
#include "Memory/Memory.hpp"

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
union ProgramStatusRegisters
{
    struct FlagsStruct
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

// Define shorthands for the most commonly used registers
#define SP 13
#define LR 14
#define PC 15

struct CPUState
{
    int32_t Registers[16];
    ProgramStatusRegisters CPSR; // Current Program Status Register
    ProgramStatusRegisters SPSR; // Saved Program Status Register
};

class CPU final
{
public:
    CPU(CPUMode mode);
    void LoadROM(GBAHeader& header, FILE* rom);
    void Reset();
    void Run();

    InstructionSet GetCurrentInstructionSet() { return InstructionSet(_state.CPSR.Flags.T); }
    int32_t& GetRegister(uint8_t reg) { return _state.Registers[reg]; }
    void SetInstructionSet(InstructionSet set) { _state.CPSR.Flags.T = uint8_t(set); }
    void ToggleInstructionSet() { _state.CPSR.Flags.T ^= 1; }
    ProgramStatusRegisters::FlagsStruct& GetCurrentStatusFlags() { return _state.CPSR.Flags; }

private:
    CPUMode _mode;
    CPURunState _runState;
    CPUState _state;
    std::unique_ptr<Interpreter> _interpreter;
    std::unique_ptr<Decoder> _decoder;
    std::unique_ptr<MMU> _memory;
};

#endif
