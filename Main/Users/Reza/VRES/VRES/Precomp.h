#pragma once

#define NOMINMAX
#include <Windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <OVR.h>
#include <..\Src\OVR_CAPI.h>
#define OVR_D3D_VERSION 11
#include <..\Src\OVR_CAPI_D3D.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <cmath>

#include <memory>
#include <map>
#include <hash_set>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <string>
