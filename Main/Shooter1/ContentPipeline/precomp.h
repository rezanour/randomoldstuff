#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

// DirectX and Math
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

using namespace DirectX;

#include <mmreg.h>

// File importer
#include <ImportFile.h>

// STL
#include <algorithm>
#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <atomic>

// PPL
#include <ppl.h>
#include <ppltasks.h>

#include <debug.h>
#include <propsack.h>

#include "config.h"
#include "outputmgr.h"
#include "taskmanager.h"
#include "texture.h"
