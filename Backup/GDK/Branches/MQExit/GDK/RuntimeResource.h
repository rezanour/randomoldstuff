// Subsystems implement this for their resources, which allows them to be properly managed by the runtime content system
#pragma once

#include "Platform.h"
#include "RefCounted.h"
#include "ContentTypes.h"
#include "ContentTag.h"

namespace GDK
{
    struct IRuntimeResource : public IRefCounted
    {
        GDK_METHOD_(uint64) GetContentId() const = 0;
        GDK_METHOD_(bool) IsLoaded() const = 0;

        GDK_METHOD Load() = 0;
        GDK_METHOD Unload() = 0;
    };
}
