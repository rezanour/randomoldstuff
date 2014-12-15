#include "precomp.h"
#include "shadowmaps.h"
#include "lighting.h"
#include "shaders.h"
#include "texture2d.h"
#include "camera.h"
#include "scene.h"
#include "vertex.h"
#include "object.h"
#include "blur.h"

DirectionalLightShadows::DirectionalLightShadows(ID3D11DeviceContext* context) :
    _context(context)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsDirectionalShadow.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psDirectionalShadow.cso");

    _vsPerLightConstantBuffer = CreateConstantBuffer<vsPerLightConstants>(device.Get());
    _vsPerObjectConstantBuffer = CreateConstantBuffer<vsPerObjectConstants>(device.Get());

    _shadowDepth.reset(new Texture2D(device.Get(), 512, 512, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT));
    _blurScratch.reset(new Texture2D(device.Get(), 512, 512, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, DXGI_FORMAT_R16G16_FLOAT));

    _blur.reset(new Blur(context));

    ComPtr<IUnknown> unk;
    uint32_t unkSize = sizeof(IUnknown*);
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &unkSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> byteCode;
    unk.As(&byteCode);

    D3D11_INPUT_ELEMENT_DESC elems[3] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(VertexFormat::PositionNormalTexture, elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }
}

void DirectionalLightShadows::BuildShadowmaps(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    _context->IASetInputLayout(_inputLayout.Get());

    uint32_t strides[] = { sizeof(VertexPositionNormalTexture) };
    uint32_t offsets[] = { 0 };
    float clearShadow[] = { 1, 1, 1, 1 };

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    ID3D11Buffer* constantBuffers[] = { _vsPerLightConstantBuffer.Get(), _vsPerObjectConstantBuffer.Get() };
    _context->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = 512.0f;
    viewport.Height = 512.0f;
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);

    scene->GetLightsInSphere(XMFLOAT3(), 1, _cachedLights);
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Directional)
        {
            auto directionalLight = reinterpret_cast<DirectionalLight*>(light.get());

            if (directionalLight->ShadowMap == nullptr)
            {
                directionalLight->ShadowMap.reset(new Texture2D(device.Get(), 512, 512, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16_FLOAT));
            }

            XMVECTOR dir = XMLoadFloat3(&directionalLight->Direction);
            XMVECTOR up = XMVector3Cross(dir, XMVector3Cross(XMVectorSet(0, 1, 0, 0), dir));

            XMVECTOR det;
            XMMATRIX invView = XMMatrixInverse(&det, XMLoadFloat4x4(&camera->GetView()));
            XMVECTOR cameraPosition = invView.r[3];
            XMVECTOR cameraDir = invView.r[2];
            XMVECTOR lightTarget = XMVectorAdd(cameraPosition, XMVectorScale(cameraDir, 5));
            XMVECTOR lightPos = XMVectorAdd(lightTarget, XMVectorScale(dir, 10));
            XMMATRIX view = XMMatrixLookAtLH(lightPos, lightTarget, up);
            XMMATRIX proj = XMMatrixOrthographicLH(10, 10, 1, 30);

            XMStoreFloat4x4(&_vsPerLightConstants.View, view);
            XMStoreFloat4x4(&_vsPerLightConstants.Projection, proj);
            XMStoreFloat4x4(&directionalLight->View, view);
            XMStoreFloat4x4(&directionalLight->Projection, proj);

            UpdateConstantBuffer(_context.Get(), _vsPerLightConstantBuffer.Get(), _vsPerLightConstants);

            ID3D11RenderTargetView* rts[] = { directionalLight->ShadowMap->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
            _context->OMSetRenderTargets(_countof(rts), rts, _shadowDepth->GetDepthStencilView().Get());

            _context->ClearRenderTargetView(directionalLight->ShadowMap->GetRenderTargetView().Get(), clearShadow);
            _context->ClearDepthStencilView(_shadowDepth->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            scene->GetObjectsInSphere(XMFLOAT3(), 1, _cachedObjects);
            for (auto object : _cachedObjects)
            {
                _vsPerObjectConstants.World = object->GetWorld();
                UpdateConstantBuffer(_context.Get(), _vsPerObjectConstantBuffer.Get(), _vsPerObjectConstants);

                _context->IASetIndexBuffer(object->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
                _context->IASetVertexBuffers(0, 1, object->GetVertexBuffer().GetAddressOf(), strides, offsets);
                _context->DrawIndexed(object->GetIndexCount(), 0, 0);
            }
            _cachedObjects.clear();

            ID3D11RenderTargetView* rtsNull[] = { nullptr, nullptr, nullptr, nullptr };
            _context->OMSetRenderTargets(_countof(rtsNull), rtsNull, nullptr);
        }
    }
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Directional)
        {
            auto directionalLight = reinterpret_cast<DirectionalLight*>(light.get());
            _blur->BlurSurface(directionalLight->ShadowMap, _blurScratch);
            //_blur->BlurSurface(directionalLight->ShadowMap, _blurScratch);
            //_context->GenerateMips(directionalLight->ShadowMap->GetShaderResourceView().Get());
        }
    }

    _cachedLights.clear();
}

PointLightShadows::PointLightShadows(ID3D11DeviceContext* context) :
    _context(context)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsPointShadow.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psPointShadow.cso");

    _vsPerLightConstantBuffer = CreateConstantBuffer<vsPerLightConstants>(device.Get());
    _vsPerObjectConstantBuffer = CreateConstantBuffer<vsPerObjectConstants>(device.Get());

    _shadowDepth.reset(new Texture2D(device.Get(), 1024, 1024, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT));
    _blurScratch.reset(new Texture2D(device.Get(), 1024, 1024, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, DXGI_FORMAT_R16G16_FLOAT));

    _blur.reset(new Blur(context));

    ComPtr<IUnknown> unk;
    uint32_t unkSize = sizeof(IUnknown*);
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &unkSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> byteCode;
    unk.As(&byteCode);

    D3D11_INPUT_ELEMENT_DESC elems[3] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(VertexFormat::PositionNormalTexture, elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }
}

void PointLightShadows::BuildShadowmaps(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    _context->IASetInputLayout(_inputLayout.Get());

    uint32_t strides[] = { sizeof(VertexPositionNormalTexture) };
    uint32_t offsets[] = { 0 };
    float clearShadow[] = { 1, 1, 1, 1 };

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    ID3D11Buffer* constantBuffers[] = { _vsPerLightConstantBuffer.Get(), _vsPerObjectConstantBuffer.Get() };
    _context->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = 1024.0f;
    viewport.Height = 1024.0f;
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);

    scene->GetLightsInSphere(XMFLOAT3(), 1, _cachedLights);
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Point)
        {
            auto pointLight = reinterpret_cast<PointLight*>(light.get());

            if (pointLight->ShadowMapFront == nullptr)
            {
                pointLight->ShadowMapFront.reset(new Texture2D(device.Get(), 1024, 1024, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16_FLOAT));
            }
            if (pointLight->ShadowMapBack == nullptr)
            {
                pointLight->ShadowMapBack.reset(new Texture2D(device.Get(), 1024, 1024, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16_FLOAT));
            }

            // Front
            XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pointLight->Position), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 1, 0, 0));
            XMStoreFloat4x4(&_vsPerLightConstants.View, view);
            _vsPerLightConstants.ParaboloidNear = 0.01f;
            _vsPerLightConstants.ParaboloidFar = pointLight->Radius;
            UpdateConstantBuffer(_context.Get(), _vsPerLightConstantBuffer.Get(), _vsPerLightConstants);

            ID3D11RenderTargetView* rts[] = { pointLight->ShadowMapFront->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
            _context->OMSetRenderTargets(_countof(rts), rts, _shadowDepth->GetDepthStencilView().Get());

            _context->ClearRenderTargetView(pointLight->ShadowMapFront->GetRenderTargetView().Get(), clearShadow);
            _context->ClearDepthStencilView(_shadowDepth->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            scene->GetObjectsInSphere(XMFLOAT3(), 1, _cachedObjects);
            for (auto object : _cachedObjects)
            {
                _vsPerObjectConstants.World = object->GetWorld();
                UpdateConstantBuffer(_context.Get(), _vsPerObjectConstantBuffer.Get(), _vsPerObjectConstants);

                _context->IASetIndexBuffer(object->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
                _context->IASetVertexBuffers(0, 1, object->GetVertexBuffer().GetAddressOf(), strides, offsets);
                _context->DrawIndexed(object->GetIndexCount(), 0, 0);
            }
            _cachedObjects.clear();

            // Back
            view = XMMatrixLookToLH(XMLoadFloat3(&pointLight->Position), XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 1, 0, 0));
            XMStoreFloat4x4(&_vsPerLightConstants.View, view);
            UpdateConstantBuffer(_context.Get(), _vsPerLightConstantBuffer.Get(), _vsPerLightConstants);

            rts[0] = pointLight->ShadowMapBack->GetRenderTargetView().Get();
            _context->OMSetRenderTargets(_countof(rts), rts, _shadowDepth->GetDepthStencilView().Get());

            _context->ClearRenderTargetView(pointLight->ShadowMapBack->GetRenderTargetView().Get(), clearShadow);
            _context->ClearDepthStencilView(_shadowDepth->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            scene->GetObjectsInSphere(XMFLOAT3(), 1, _cachedObjects);
            for (auto object : _cachedObjects)
            {
                _vsPerObjectConstants.World = object->GetWorld();
                UpdateConstantBuffer(_context.Get(), _vsPerObjectConstantBuffer.Get(), _vsPerObjectConstants);

                _context->IASetIndexBuffer(object->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
                _context->IASetVertexBuffers(0, 1, object->GetVertexBuffer().GetAddressOf(), strides, offsets);
                _context->DrawIndexed(object->GetIndexCount(), 0, 0);
            }
            _cachedObjects.clear();

            ID3D11RenderTargetView* rtsNull[] = { nullptr, nullptr, nullptr, nullptr };
            _context->OMSetRenderTargets(_countof(rtsNull), rtsNull, nullptr);
        }
    }
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Point)
        {
            //auto pointLight = reinterpret_cast<PointLight*>(light.get());
            //_blur->BlurSurface(pointLight->ShadowMapFront, _blurScratch);
            //_blur->BlurSurface(pointLight->ShadowMapBack, _blurScratch);
        }
    }

    _cachedLights.clear();
}
