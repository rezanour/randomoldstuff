#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_INTERNAL_SDK_H_
#define _GDK_INTERNAL_SDK_H_

// Rollup header to simplify including internal SDK.
// Just include this header as a convenience instead
// of all the others.

// GDK Public
#include <GDK\Platform.h>
#include <GDK\RefCounted.h>

// STL
#include <exception>
#include <vector>

// STDE
#include <stde\ref_ptr.h>

// GDK Internal SDK
#include "Error.h"
#include "Engine.h"
#include "Hosting.h"
#include "Graphics.h"
#include "MemoryStats.h"

#endif // _GDK_INTERNAL_SDK_H_
