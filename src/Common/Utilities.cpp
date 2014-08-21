#include "Utilities.hpp"

#include <iostream>

void Utilities::Assert(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << std::endl << "ASSERTION FAILURE:" << std::endl;
        std::cerr << " --> what(): " << message << std::endl << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

