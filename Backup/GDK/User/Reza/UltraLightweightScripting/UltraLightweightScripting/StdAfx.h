#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <cassert>

#include <new>
#include <string>
#include <map>
#include <vector>

#include <strsafe.h>

using std::size_t;
using std::min;
using std::max;

typedef std::uint8_t byte_t;

#define STRINGIFY(z) #z
#define TOSTRING(y) STRINGIFY(y)
#define GDKNEW(x) new (x " allocated on line " TOSTRING(__LINE__) " of "__FILE__)

template <typename Ty>
inline void SafeRelease(Ty*& p)
{
    if (p)
    {
        p->Release();
        p = nullptr;
    }
}

template <typename Ty>
inline void SafeDelete(Ty*& p)
{
    if (p)
    {
        delete p;
        p = nullptr;
    }
}

template <typename Ty>
inline void SafeDeleteArray(Ty*& p)
{
    if (p)
    {
        delete[] p;
        p = nullptr;
    }
}
