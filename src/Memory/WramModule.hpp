#ifndef WRAM_MODULE_HPP
#define WRAM_MODULE_HPP

#include "MemoryModule.hpp"

class WRAM final : public MemoryModule
{
    public:
        WRAM(uint32_t size) : MemoryModule(size) { }

};

#endif