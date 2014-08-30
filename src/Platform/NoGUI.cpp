#include "NoGUI.hpp"
#include "Common/GBA.hpp"

#include <iostream>

NoGUI::NoGUI(int argc, char* argv[])
{
    FILE* rom = fopen(argv[2], "rb");

    if (!rom)
    {
        std::cout << "Could not load specified ROM file" << std::endl;
        return;
    }

    GBAHeader header;

    int read = fread(&header, sizeof(GBAHeader), 1, rom);

    if (!read)
    {
        std::cout << "Could not read ROM header." << std::endl;
        fclose(rom);
        return;
    }
    
    if (!header.VerifyHeader())
    {
        std::cout << "The specified file is not a valid GBA ROM." << std::endl;
        fclose(rom);
        return;
    }

    FILE* bios = fopen("E:/NDS_Projects/gba_bios.bin", "rb");
    if (!bios)
    {
        std::cout << "Could not load the GBA Bios." << std::endl;
        return;
    }

    _cpu = std::unique_ptr<CPU>(new CPU(CPUExecutionMode::Interpreter));

    RegisterCPUCallbacks();

    _cpu->LoadROM(header, rom, bios);

    fclose(bios);
    fclose(rom);
}

void NoGUI::RegisterCPUCallbacks()
{
    _cpu->RegisterInstructionCallback(InstructionCallbackTypes::InstructionExecuted, [&](std::shared_ptr<Instruction> instruction)
    {
        std::cout << "Set: " << (instruction->GetInstructionSet() == InstructionSet::ARM ? "ARM" : "Thumb") << ". Instruction: " << instruction->ToString() << std::endl;
    });
}

