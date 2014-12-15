#pragma once

#include <Windows.h>
#include <d3d11.h>

// STL isn't warning level 4 clean, so we lower warning level temporarily
#pragma warning(push, 3)
#pragma warning(disable: 4995)
#include <string>
#include <memory>
#include <vector>
#include <map>
#pragma warning(pop)

#define DIRECTXMATH_INTEROP
#include <Lucid3D.Math.h>

#include <stde\types.h>
#include <stde\com_ptr.h>
#include <stde\ref_counted.h>
#include <stde\non_copyable.h>
#include <stde\conversion.h>

#include <CoreServices\Debug.h>

