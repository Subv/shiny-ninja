#ifndef NO_GUI_HPP
#define NO_GUI_HPP

#include "CPU/CPU.hpp"

#include <memory>

class NoGUI
{
public:
    NoGUI(int argc, char* argv[]);

    void Run() { _cpu->Run(); }
    void RegisterCPUCallbacks();

private:
    std::unique_ptr<CPU> _cpu;
};
#endif