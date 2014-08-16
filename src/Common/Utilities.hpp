#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#undef _GLIBCXX_HAVE_BROKEN_VSWPRINTF

namespace Utilities
{
    void Assert(bool condition, const char* message);
}

#endif