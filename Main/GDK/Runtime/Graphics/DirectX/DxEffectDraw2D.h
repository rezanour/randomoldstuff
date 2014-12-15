#pragma once

#include "DxEffect.h"

namespace GDK
{
    class DxEffectDraw2D : public DxEffect
    {
    public:
        static std::shared_ptr<Effect> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);

        virtual void Draw(_In_ const std::shared_ptr<Geometry>& geometry, _In_ const Matrix& world, _In_ uint32_t frame) override;

    private:
        DxEffectDraw2D(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice);

        struct VSConstants
        {
            Vector2 WorkAreaSize;
            Vector2 ViewAreaSize;
        } _vsConstants;
    };
}
