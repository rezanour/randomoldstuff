#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// STL isn't warning level 4 clean, so we lower warning level temporarily
#pragma warning(push, 3)
#pragma warning(disable: 4995)
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <csignal>
#include <type_traits>
#pragma warning(pop)

#include <strsafe.h>

#include <stde\types.h>
#include <stde\conversion.h>
#include <stde\non_copyable.h>
#include <stde\com_ptr.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <GDK\Tools\geometryresource.h>
#include <GDK\Tools\textureresource.h>


