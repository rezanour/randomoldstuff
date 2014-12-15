#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
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

// DirectX and Math
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// DirectXTK Audio
#include <Audio.h>

using namespace DirectX;

// File importer
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
#include <deque>
#include <filesystem>
#include <fstream>

//
// Helper to do enum flag checks without ugly casts all over the code
//

template <typename TEnum>
inline TEnum FlagCombine(TEnum v1, TEnum v2)
{
    return (TEnum)((uint32_t)v1 | (uint32_t)v2);
}

template <typename TEnum>
inline bool IsFlagSet(TEnum flags, TEnum value)
{
    return ((uint32_t)flags & (uint32_t)value) != 0;
}

template <typename TEnum>
inline bool IsFlagClear(TEnum flags, TEnum value)
{
    return ((uint32_t)flags & (uint32_t)value) == 0;
}

template <typename TEnum>
inline bool AreAllFlagsSet(TEnum flags, TEnum values)
{
    return ((uint32_t)flags & (uint32_t)values) == (uint32_t)values;
}

template <typename TEnum>
inline bool AreAllFlagsClear(TEnum flags, TEnum values)
{
    return ((uint32_t)flags & (uint32_t)values) == 0;
}

#include <debug.h>
#include <propsack.h>

#include "memorytracking.h"
#include "trackedobject.h"
#include "config.h"
#include "animation.h"
#include "aabb.h"
#include "scene.h"
#include "gameobject.h"
#include "graphics.h"
#include "texturepool.h"
#include "geopool.h"
#include "texture.h"
#include "geometry.h"
#include "material.h"
#include "spritefont.h"
#include "singlesoundeffect.h"
#include "assetloader.h"
#include "input.h"
#include "content.h"
#include "game.h"
#include "menu.h"
#include "audiomenu.h"
#include "controlsmenu.h"
#include "hudmenu.h"
#include "graphicsmenu.h"
#include "optionsmenu.h"
#include "gamemenu.h"
#include "mainmenu.h"
#include "gamestates.h"
#include "gameplaystates.h"
#include "gameplay.h"
