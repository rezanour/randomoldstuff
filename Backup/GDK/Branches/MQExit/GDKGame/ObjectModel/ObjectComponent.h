#pragma once

#include <stde\non_copyable.h>
#include <GDK\ObjectModel\ObjectComponent.h>

namespace GDK
{
    class ObjectComponent : stde::non_copyable, public GDK::RefCountedBase<GDK::IObjectComponent>
    {
    };
}

