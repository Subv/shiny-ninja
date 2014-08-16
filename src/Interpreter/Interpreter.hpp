#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Common/Instructions/ARMInstruction.hpp"
#include "Common/Instructions/ThumbInstruction.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

class CPU;

class Interpreter
{
public:
    Interpreter(std::shared_ptr<CPU> arm) : _cpu(arm) { InitializeHandlers(); }

    void RunInstruction(std::shared_ptr<Instruction> instruction);
    
    void HandleARM(std::shared_ptr<ARMInstruction> instruction);
    void HandleThumb(std::shared_ptr<ThumbInstruction> instruction);
    
    // ARM Instruction handlers
    void HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction);

private:
    std::shared_ptr<CPU> _cpu;
    std::unordered_map<ARM::ARMOpcodes, std::function<void(std::shared_ptr<ARMInstruction>)>, std::hash<int>> _armHandlers;
    std::unordered_map<Thumb::ThumbOpcodes, std::function<void(std::shared_ptr<ThumbInstruction>)>, std::hash<int>> _thumbHandlers;
    void InitializeHandlers();
};
#endif