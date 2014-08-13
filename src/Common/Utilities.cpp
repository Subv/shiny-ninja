#include "Utilities.hpp"

#include <iostream>

void Utilities::Assert(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << message << std::endl;
        std::cin.get();
        std::terminate();
    }
}

