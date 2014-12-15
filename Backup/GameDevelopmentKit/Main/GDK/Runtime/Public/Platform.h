#pragma once

#include <stddef.h>
#include <stdint.h>

#include <exception>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <map>

#include "XPlatSal.h"

typedef uint8_t byte_t;

// TODO: Find a better place for these common structs

#pragma pack(push, 1)   // these structures are used in file I/O, and so must be packed to 1 byte packing

struct RectangleF
{
    RectangleF() : left(0), top(0), width(0), height(0) {}
    RectangleF(_In_ float left, _In_ float top, _In_ float width, _In_ float height) : left(left), top(top), width(width), height(height) {}

    float left;
    float top;
    float width;
    float height;
};

#pragma pack(pop)

// sprintf_s is required on Windows to avoid warnings about unsafe CRT functions.
// There is a templated version that infers the size of the array from the first parameter
// and therefore has the same effective signature as sprintf, so we can safely #define them this way
// for other platforms.
// Same applies to memcpy & memcpy_s
#ifndef WIN32
#define sprintf_s sprintf
#endif

#define WIDEN2(x) L###x
#define WIDEN(x) WIDEN2(x)

// TODO: Nick can you verify that other platforms don't actually have __FILEW__? If they do, we can remove this
#ifndef WIN32
#define __FILEW__ WIDEN(__FILE__)
#endif

#define __FUNCSIGW__ WIDEN(__FUNCSIG__)

#ifdef WIN32
#define UNREFERENCED_PARAMETER(x) (x)
#else
#define UNREFERENCED_PARAMETER(x)
#endif

#define IGNORE_RETURN   (void)

#if defined (DEBUG) || defined (_DEBUG)
#define GDKDEBUG 1
#endif
