// math.h
// A collection of useful and/or optimized math functionality

#pragma once

#undef min
#undef max

namespace stde
{
    // returns -1 if f is negative, or 1 if it is positive
    template <typename T>
    inline T __fastcall sign(const T& value)
    {
        return value < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1);
    }

    // returns the absolute value of f
    template <typename T>
    inline T __fastcall abs(const T& value)
    {
        return value < static_cast<T>(0) ? -value : value;
    }

    // strongly typed min function
    template <typename T>
    inline T __fastcall min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    // strongly typed max function
    template <typename T>
    inline T __fastcall max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }
}

using stde::min;
using stde::max;
