#pragma once

#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <wincodec.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <algorithm>

//#define ENABLE_VR
#include "vrsupport.h"
