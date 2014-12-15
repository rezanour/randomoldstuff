#pragma once

#include <GDK\RefCounted.h>
#include <GDK\ObjectModel\ObjectComponent.h>

#include "GraphicsTypes.h"
#include <stde\non_copyable.h>

#include "CameraComponent.h"
#include "RenderTask.h"

namespace Lucid3D
{
    class BaseVisualComponent;
    typedef stde::ref_counted_ptr<BaseVisualComponent> BaseVisualPtr;

    class BaseVisualComponent : stde::non_copyable, public GDK::RefCounted<GDK::IVisualComponent>
    {
    public:
        GDK_IMETHOD Process(_Inout_ RenderMap& renderMap) = 0;

    protected:
        BaseVisualComponent();
    };
}
