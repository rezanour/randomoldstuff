#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_COMMON_H_
#define _GDK_COMMON_H_

////////////////////////////////////////////////////////////////////////////////
// Pull in most commonly used parts of the GDK engine

// Platform
#include "Platform.h"
#include "Error.h"

// Engine
#include "RefCounted.h"
#include "Hosting.h"
#include "EngineCore.h"
#include "Message.h"

#endif // _GDK_COMMON_H_
