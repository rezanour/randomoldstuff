#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>
#include <comdef.h>
#include <intrin.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

// DDS & WIC support
#include <DirectXTex.h>

// DirectX and Math
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <SimpleMath.h>

// DirectXTK Audio
#include <Audio.h>
#include <WavFileReader.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <ImportFile.h>

// STL
#include <algorithm>
#include <array>
#include <exception>
#include <functional>
#include <hash_map>
#include <hash_set>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

// Common shooter1 headers
#include <debug.h>

#include "utilities.h"