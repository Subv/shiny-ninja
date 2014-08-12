#include "Files.hpp"
#include <cstdio>

namespace Files
{
    uint32_t GetFileSize(FILE* file)
    {
        uint32_t cur = ftell(file);
        fseek(file, 0, SEEK_END);
        uint32_t size = ftell(file);
        fseek(file, cur, SEEK_SET);
        return size;
    }
}

