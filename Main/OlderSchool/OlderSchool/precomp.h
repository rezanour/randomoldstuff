#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DirectXMath.h>

#include <d3d11.h>
#include <wrl.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <array>
#include <exception>
#include <functional>
#include <hash_map>
#include <hash_set>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include "debug.h"
#include "globals.h"
