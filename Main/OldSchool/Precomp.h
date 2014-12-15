#pragma once

#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

//#define _XM_NO_INTRINSICS_
//#define _XM_VECTORCALL_ 1
#define _OSM_VECTORCALL_ 1

// Old School Math (OSM)
#include <OldSchoolMath.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <d3d11.h>

#include <wincodec.h>

#include <OVR.h>

#include <strsafe.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdexcpt.h>
#include <math.h>

#include <limits>
#include <numeric>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <hash_map>
#include <hash_set>
#include <string>
#include <exception>
#include <algorithm>

#include "stlextensions.h"

#include "Profile.h"

#include "BaseObject.h"
#include "Transform.h"
#include "AABB.h"
#include "Object.h"
