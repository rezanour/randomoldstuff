#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#include <wrl.h>

#include <memory>
#include <cassert>
#include <string>
#include <map>
#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

#include "q2debug.h"
#include "q2pak.h"
#include "q2bsp.h"
#include "q2render.h"
#include "q2wal.h"

#define RIP(format, ...)                \
    {                                   \
        DbgPrintf(format, __VA_ARGS__); \
        if (IsDebuggerPresent())        \
            DebugBreak();               \
        exit(-1);                       \
    }
