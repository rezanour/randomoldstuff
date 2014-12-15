#pragma once

#include <stde\non_copyable.h>
#include <GDK\ObjectModel\ObjectComponent.h>

namespace Lucid3D
{
    class ObjectComponent : stde::non_copyable, public GDK::RefCountedBase<GDK::IObjectComponent>
    {
    };
}

