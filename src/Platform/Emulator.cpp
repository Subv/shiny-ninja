#include "Common/GBA.hpp"
#include "CPU/CPU.hpp"
#include "Common/Utilities.hpp"
#include <QApplication>
#include "MainWindow.h"

#include <iostream>
#include <cstdio>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "You must specify a ROM file to load." << std::endl;
        return 0;
    }

    FILE* rom = fopen(argv[1], "rb");
    // FILE* rom = fopen("pkmn.gba", "rb"); // Fo' testin'

    if (!rom)
    {
        std::cout << "The file you have specified is invalid." << std::endl;
        return 0;
    }

    GBAHeader header;
    int read = fread(&header, sizeof(GBAHeader), 1, rom);

    if (!read)
    {
        std::cout << "Could not read ROM header." << std::endl;
        fclose(rom);
        return -1;
    }

    if (!header.VerifyHeader())
    {
        std::cout << "The specified file is not a valid GBA ROM." << std::endl;
        fclose(rom);
        return -2;
    }

    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    app.exec();

    std::shared_ptr<CPU> arm7(new CPU(CPUExecutionMode::Interpreter));
    arm7->LoadROM(header, rom);

    fclose(rom);

    arm7->Run();

    return 0;
}
