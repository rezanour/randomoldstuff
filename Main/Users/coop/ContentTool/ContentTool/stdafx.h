#pragma once

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <d3d11.h>

#include <wincodec.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdexcpt.h>

#include <limits>
#include <numeric>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <exception>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "gdkcompat.h"
#include "helpers.h"
#include "file.h"
#include "pak.h"
#include "bmp.h"
#include "pcx.h"
#include "wal.h"
#include "bsp.h"
#include "md2.h"
#include "wl6.h"
#include "obj.h"
