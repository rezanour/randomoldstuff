#pragma once

#include "DxEffect.h"

namespace GDK
{
    class DxEffectStatic3D : public DxEffect
    {
    public:
        static std::shared_ptr<Effect> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);

        virtual void Draw(_In_ const std::shared_ptr<Geometry>& geometry, _In_ const Matrix& world, _In_ uint32_t frame) override;

    private:
        DxEffectStatic3D(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);

        struct DirectionalLight
        {
            Vector4 Direction;
            Vector4 Color;
        };

        struct VSConstants
        {
            Matrix World;
            Matrix InvTransWorld;
            Matrix ViewProj;
        } _vsConstants;

        struct PSConstants
        {
            DirectionalLight Light1;
            DirectionalLight Light2;
            DirectionalLight Light3;
        } _psConstants;
    };
}
