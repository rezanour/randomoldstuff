#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX;

#include <d3d11.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdexcpt.h>

#include <limits>
#include <numeric>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <exception>

#include "BaseObject.h"
#include "Transform.h"
#include "Common.h"

