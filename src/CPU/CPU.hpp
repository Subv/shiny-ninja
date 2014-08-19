#ifndef CPU_HPP
#define CPU_HPP

#include "Decoder/Decoder.hpp"
#include "Interpreter/Interpreter.hpp"
#include "Memory/Memory.hpp"

#include <atomic>
#include <cstdio>
#include <array>

struct GBAHeader;

enum class CPUExecutionMode
{
    Interpreter,
    JIT
};

enum class CPUMode
{
    User = 0x10,
    FIQ = 0x11,
    IRQ = 0x12,
    Supervisor = 0x13,
    Abort = 0x17,
    Undefined = 0x1B,
    System = 0x1F
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

enum class InstructionCallbackTypes
{
    InstructionDecoded,
    InstructionExecuted
};

// We have to provide a std::hash specialization for this enum if we want to use it in an unordered_map
namespace std
{
    template<>
    struct hash<InstructionCallbackTypes>
    {
        typedef InstructionCallbackTypes argument_type;
        typedef std::underlying_type<argument_type>::type underlying_type;
        typedef std::hash<underlying_type>::result_type result_type;
        result_type operator()(const argument_type& arg) const
        {
            std::hash<underlying_type> hasher;
            return hasher(static_cast<underlying_type>(arg));
        }
    };
}

// Used by the MSR and MRS instructions
// These are different in each ARM version, we are using the values from the ARMv4T architecture
enum BitMaskConstants
{
    UnallocMask = 0x0FFFFF00,
    UserMask = 0xF0000000,
    PrivMask = 0x0000000F,
    StateMask = 0x00000020
};

// Define shorthands for the most commonly used registers
#define SP 13
#define LR 14
#define PC 15

struct CPUState
{
    std::array<int32_t, 16> Registers;

    // Banked registers
    std::array<int32_t, 7> Registers_FIQ; // Contains R8-R14 for the FIQ mode
    std::array<int32_t, 2> Registers_svc; // Contains R13-R14 for the Supervisor mode
    std::array<int32_t, 2> Registers_abt; // Contains R13-R14 for the Abort mode
    std::array<int32_t, 2> Registers_IRQ; // Contains R13-R14 for the IRQ mode
    std::array<int32_t, 2> Registers_und; // Contains R13-R14 for the Undefined mode

    ProgramStatusRegisters CPSR; // Current Program Status Register
    ProgramStatusRegisters SPSR[5]; // Saved Program Status Register, there is one per mode except in System/User mode
};

class CPU final
{
public:
    CPU(CPUExecutionMode mode);
    void LoadROM(GBAHeader& header, FILE* rom);
    void Reset();
    void Stop() { _runState = CPURunState::Stopped; }
    void Run();

    bool ConditionPasses(InstructionCondition condition);

    int32_t& GetRegister(uint8_t reg);
    int32_t& GetRegisterForMode(CPUMode mode, uint8_t reg);

    InstructionSet GetCurrentInstructionSet() const { return InstructionSet(_state.CPSR.Flags.T); }
    void SetInstructionSet(InstructionSet set) { _state.CPSR.Flags.T = uint8_t(set); }
    void ToggleInstructionSet() { _state.CPSR.Flags.T ^= 1; }

    ProgramStatusRegisters::FlagsStruct& GetCurrentStatusFlags() { return _state.CPSR.Flags; }
    ProgramStatusRegisters& GetCurrentStatusRegister() { return _state.CPSR; }
    ProgramStatusRegisters& GetSavedStatusRegister();

    CPUMode GetCurrentCPUMode() const { return CPUMode(_state.CPSR.Flags.M); }
    bool IsInPrivilegedMode() const { return GetCurrentCPUMode() != CPUMode::User; }
    void SetCurrentCPUMode(CPUMode mode) { _state.CPSR.Flags.M = uint8_t(mode); }

    std::unique_ptr<MMU>& GetMemory() { return _memory; }

    void RegisterInstructionCallback(InstructionCallbackTypes type, std::function<void(std::shared_ptr<Instruction>)> callback) { _instructionCallbacks[type] = callback; }
    void ExecuteInstructionCallback(InstructionCallbackTypes type, std::shared_ptr<Instruction> instruction);

    void StepInstruction();

private:
    CPUExecutionMode _mode;
    std::atomic<CPURunState> _runState;
    CPUState _state;
    std::unique_ptr<Interpreter> _interpreter;
    std::unique_ptr<Decoder> _decoder;
    std::unique_ptr<MMU> _memory;

    // Callbacks are used to inform the UI about stuff that happens in the emulator
    std::unordered_map<InstructionCallbackTypes, std::function<void(std::shared_ptr<Instruction>)>> _instructionCallbacks;
};

#endif
