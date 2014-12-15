#include "DxEffectDraw2D.h"
#include <GDKError.h>

namespace GDK
{
    std::shared_ptr<Effect> DxEffectDraw2D::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    {
        return std::shared_ptr<Effect>(GDKNEW DxEffectDraw2D(graphicsDevice));
    }

    DxEffectDraw2D::DxEffectDraw2D(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice) :
        DxEffect(graphicsDevice)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> elems;
        D3D11_INPUT_ELEMENT_DESC elem = {};

        elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        elem.SemanticName = "POSITION";
        elem.Format = DXGI_FORMAT_R32G32_FLOAT;
        elems.push_back(elem);

        elem.SemanticName = "TEXCOORD";
        elem.Format = DXGI_FORMAT_R32G32_FLOAT;
        elem.AlignedByteOffset = 8;
        elems.push_back(elem);

        CreateVertexShaderAndInputLayout(L"Draw2DVS.cso", elems);
        CreatePixelShader(L"Draw2DPS.cso");
        CreateSampler(FilterType::Linear, AddressMode::Wrap, AddressMode::Wrap);
        CreateBlendState(BlendMode::AlphaBlend);
        CreateVSConstantBuffer(sizeof(_vsConstants));
    }

    void DxEffectDraw2D::Draw(_In_ const std::shared_ptr<Geometry>& geometry, _In_ const Matrix& world, _In_ uint32_t frame)
    {
        UNREFERENCED_PARAMETER(world);
        CHECK_TRUE(geometry->GetType() == Geometry::Type::Geometry2D);

        auto graphicsDevice = LockGraphicsDevice();
        auto graphicsDeviceBase = static_cast<GraphicsDeviceBase*>(graphicsDevice.get());

        _vsConstants.WorkAreaSize = graphicsDeviceBase->GetWorkArea();
        _vsConstants.ViewAreaSize = graphicsDeviceBase->GetViewArea();
        UpdateVSConstantBuffer(&_vsConstants, sizeof(_vsConstants));

        DrawGeometry(geometry, frame);
    }
}
