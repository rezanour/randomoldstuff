#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

// STL
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <set>

// DirectX and Math
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include "debug.h"
#include "propsack.h"
