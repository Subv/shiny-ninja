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
    Interpreter(CPU* arm);

    void RunInstruction(std::shared_ptr<Instruction> instruction);
    
    void HandleARM(std::shared_ptr<ARMInstruction> instruction);
    void HandleThumb(std::shared_ptr<ThumbInstruction> instruction);
    
    // ARM Instruction handlers
    void HandleARMBranchInstruction(std::shared_ptr<ARMInstruction> instruction);
    void HandleARMDataProcessingInstruction(std::shared_ptr<ARMInstruction> instruction);
    void HandleARMLoadStoreInstruction(std::shared_ptr<ARMInstruction> instruction);
    void HandleARMMiscellaneousLoadStoreInstruction(std::shared_ptr<ARMInstruction> instruction);
    void HandleARMPSROperationInstruction(std::shared_ptr<ARMInstruction> instruction);

private:
    CPU* _cpu;
    std::unordered_map<ARM::ARMOpcodes, std::function<void(std::shared_ptr<ARMInstruction>)>, std::hash<int>> _armHandlers;
    std::unordered_map<Thumb::ThumbOpcodes, std::function<void(std::shared_ptr<ThumbInstruction>)>, std::hash<int>> _thumbHandlers;
    void InitializeHandlers();
    void InitializeArm();
    void InitializeThumb();
};
#endif
