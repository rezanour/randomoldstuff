#include "DxEffectStatic3D.h"
#include <GDKError.h>

namespace GDK
{
    std::shared_ptr<Effect> DxEffectStatic3D::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    {
        return std::shared_ptr<Effect>(GDKNEW DxEffectStatic3D(graphicsDevice));
    }

    DxEffectStatic3D::DxEffectStatic3D(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice) :
        DxEffect(graphicsDevice)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> elems;
        D3D11_INPUT_ELEMENT_DESC elem = {};

        elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        elem.SemanticName = "POSITION";
        elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elems.push_back(elem);

        elem.SemanticName = "NORMAL";
        elem.AlignedByteOffset = 12;
        elems.push_back(elem);

        elem.SemanticName = "TEXCOORD";
        elem.Format = DXGI_FORMAT_R32G32_FLOAT;
        elem.AlignedByteOffset = 24;
        elems.push_back(elem);

        CreateVertexShaderAndInputLayout(L"StaticGeometryVS.cso", elems);
        CreatePixelShader(L"TexturedPS.cso");
        CreateSampler(FilterType::Linear, AddressMode::Wrap, AddressMode::Wrap);
        CreateBlendState(BlendMode::Opaque);
        CreateVSConstantBuffer(sizeof(_vsConstants));
        CreatePSConstantBuffer(sizeof(_psConstants));

        // Light1 (bright white downward angle)
        _psConstants.Light1.Direction = Vector4(1, 1, 1, 1);
        _psConstants.Light1.Color = Vector4(1, 1, 1, 1);

        // Light2 (bright white light opposite angle)
        _psConstants.Light2.Direction = Vector4(-1, -1, -1, 1);
        _psConstants.Light2.Color = Vector4(1, 1, 1, 1);

        // Light3 (blue light shining upward from floor)
        _psConstants.Light3.Direction = Vector4(0, -1, 0, 1);
        _psConstants.Light3.Color = Vector4(1, 1, 1, 1);
    }

    void DxEffectStatic3D::Draw(_In_ const std::shared_ptr<Geometry>& geometry, _In_ const Matrix& world, _In_ uint32_t frame)
    {
        CHECK_TRUE(geometry->GetType() == Geometry::Type::Geometry3D);

        auto graphicsDevice = LockGraphicsDevice();
        auto graphicsDeviceBase = static_cast<GraphicsDeviceBase*>(graphicsDevice.get());

        _vsConstants.World = world;
        _vsConstants.ViewProj = graphicsDeviceBase->GetViewMatrix() * graphicsDeviceBase->GetProjectionMatrix();
        Matrix::Inverse(world, &_vsConstants.InvTransWorld);
        _vsConstants.InvTransWorld = Matrix::Transpose(_vsConstants.InvTransWorld);

        UpdateVSConstantBuffer(&_vsConstants, sizeof(_vsConstants));

        UpdatePSConstantBuffer(&_psConstants, sizeof(_psConstants));

        DrawGeometry(geometry, frame);
    }
}
