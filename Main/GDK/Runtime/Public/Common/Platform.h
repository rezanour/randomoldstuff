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
#include <set>
#include <hash_map>
#include <hash_set>

#include "XPlatSal.h"

typedef uint8_t byte_t;

// sprintf_s is required on Windows to avoid warnings about unsafe CRT functions.
// There is a templated version that infers the size of the array from the first parameter
// and therefore has the same effective signature as sprintf, so we can safely #define them this way
// for other platforms.
// Same applies to memcpy & memcpy_s
#ifndef WIN32
#define sprintf_s sprintf
#endif

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#ifndef __FILEW__
#ifdef __FILE__
#define __FILEW__ WIDEN(__FILE__)
#else
#error How does your compiler not have __FILE__ or __FILEW__?!
#endif
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
