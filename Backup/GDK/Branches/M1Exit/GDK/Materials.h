// Material descriptions
#pragma once

#include <stde\types.h>

namespace GDK
{
    struct MaterialProperty
    {
        enum Type
        {
            Float,
            Vector2,
            Vector3,
            Vector4,
            Color,
            ContentId,
        };

        Type Type;
        std::string Name;
        VARIANT Value;
    };

    // Extension provided descriptions, which are aggregated and exposed
    // in the toolchain. Material "instances" provide the values to fill in
    // one of these descriptions, which is then passed on to the bake step 
    // to be baked into an actual shader/effect
    struct MaterialDescription
    {
        std::string Name;
        uint32 RequiredVertexChannels;
        std::vector<MaterialProperty> Properties;
    };
}

