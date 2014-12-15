#pragma once

namespace GDK
{
    namespace ContentType
    {
        enum Enum
        {
            ObjectContent,
            ComponentContent,
            AssetContent,
            ResourceContent,
        };
    };

    namespace ObjectType
    {
        enum Enum
        {
            InvalidObject,
            ScreenObject,
            GameObjectInstanceObject,
        };
    };

    namespace ComponentType
    {
        enum Enum
        {
            InvalidComponent,
            VisualComponent,
            CameraComponent,
            LightComponent,
        };
    };

    namespace AssetType
    {
        enum Enum
        {
            InvalidAsset,
            StaticModelAsset,
            MaterialAsset,
        };
    };

    namespace ResourceType
    {
        enum Enum
        {
            InvalidResource,
            Texture2DResource,
            GeometryResource,
            VertexShaderResource,
            PixelShaderResource,
        };
    };
}

