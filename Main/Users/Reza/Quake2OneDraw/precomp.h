#pragma once

#define NOMINMAX
#include <Windows.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <DirectXMath.h>

using namespace DirectX;

#include <memory>
#include <algorithm>
#include <vector>
#include <exception>
#include <stdexcept>

#include "debug.h"
#include "pak.h"
#include "wal.h"
#include "graphics.h"
#include "bsp.h"
#include "input.h"
