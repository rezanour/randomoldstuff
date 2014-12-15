// The core subsystem interfaces for communication between GDKGame and client subsystems
#pragma once

#include "Platform.h"
#include "RefCounted.h"
#include "ContentTypes.h"
#include "ContentTag.h"
#include "ContentManager.h"

namespace GDK
{
    enum SubsystemType
    {
        GraphicsSubsystem,
        PhysicsSubsystem,
        AudioSubsystem,
    };

    struct ISubsystem : public IRefCounted
    {
        GDK_IMETHOD_(SubsystemType) GetType() const = 0;
        GDK_IMETHOD_(const char* const) GetName() const = 0;

        GDK_IMETHOD SetContentManager(_In_ IContentManager* pContentManager) = 0;
    };

    typedef HRESULT (GDK_API *pfnCreateSubsystem)(_In_ SubsystemType type, _Deref_out_ ISubsystem** ppSubsystem);
}

