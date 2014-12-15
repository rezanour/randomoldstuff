#include "precomp.h"
#include "lighting.h"
#include "shaders.h"
#include "texture2d.h"
#include "camera.h"
#include "scene.h"
#include "vertex.h"
#include "object.h"

DirectionalLighting::DirectionalLighting(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& depth, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& lights) :
    _context(context), _depth(depth), _normals(normals), _lights(lights)
{
    UNREFERENCED_PARAMETER(depth);
    UNREFERENCED_PARAMETER(normals);

    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsDirectionalLight.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psDirectionalLight.cso");

    _vsPerFrameConstantBuffer = CreateConstantBuffer<vsPerFrameConstants>(device.Get());
    _psPerFrameConstantBuffer = CreateConstantBuffer<psPerFrameConstants>(device.Get());
    _psPerLightConstantBuffer = CreateConstantBuffer<psPerLightConstants>(device.Get());

    D3D11_TEXTURE2D_DESC td = {};
    lights->GetTexture()->GetDesc(&td);
    _psPerFrameConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    if (FAILED(device->CreateSamplerState(&sd, &_pointSampler)))
    {
        throw std::exception();
    }

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    if (FAILED(device->CreateSamplerState(&sd, &_linearSampler)))
    {
        throw std::exception();
    }

    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    if (FAILED(device->CreateBlendState(&bd, &_additiveBlend)))
    {
        throw std::exception();
    }

    _quad = Object::CreateQuad(device.Get(), 0, 0, 1, 1);

    ComPtr<IUnknown> unk;
    uint32_t unkSize = sizeof(IUnknown*);
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &unkSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> byteCode;
    unk.As(&byteCode);

    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(_quad->GetVertexFormat(), elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }
}

void DirectionalLighting::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    float clearLight[] = { 0, 0, 0, 0 };
    _context->ClearRenderTargetView(_lights->GetRenderTargetView().Get(), clearLight);

    XMVECTOR det;
    XMStoreFloat4x4(&_vsPerFrameConstants.InvProjection, XMMatrixInverse(&det, XMLoadFloat4x4(&camera->GetProjection())));
    UpdateConstantBuffer(_context.Get(), _vsPerFrameConstantBuffer.Get(), _vsPerFrameConstants);

    _psPerFrameConstants.ProjectionA = camera->GetProjection()._33;
    _psPerFrameConstants.ProjectionB = camera->GetProjection()._43;
    UpdateConstantBuffer(_context.Get(), _psPerFrameConstantBuffer.Get(), _psPerFrameConstants);

    float blendAmounts[] = { 1, 1, 1, 1 };
    //_context->OMSetBlendState(_additiveBlend.Get(), blendAmounts, 0xFFFFFFFF);

    // Set buffer & nullptr for depth
    ID3D11RenderTargetView* rts[] = { _lights->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vsPerFrameConstantBuffer.GetAddressOf());

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    ID3D11Buffer* constantBuffers[] = { _psPerFrameConstantBuffer.Get(), _psPerLightConstantBuffer.Get() };
    _context->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

    // Set depth and normal maps as inputs
    ID3D11ShaderResourceView* srvs[] = { _depth->GetShaderResourceView().Get(), _normals->GetShaderResourceView().Get() };
    _context->PSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());
    _context->PSSetSamplers(1, 1, _linearSampler.GetAddressOf());

    uint32_t strides[] = { sizeof(VertexPositionNormal) };
    uint32_t offsets[] = { 0 };
    _context->IASetVertexBuffers(0, 1, _quad->GetVertexBuffer().GetAddressOf(), strides, offsets);
    _context->IASetIndexBuffer(_quad->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetInputLayout(_inputLayout.Get());

    // Draw lights
    scene->GetLightsInSphere(XMFLOAT3(), 1, _cachedLights);
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Directional)
        {
            auto directionalLight = reinterpret_cast<DirectionalLight*>(light.get());

            XMVECTOR det;

            XMVECTOR direction = XMVector3TransformNormal(XMLoadFloat3(&directionalLight->Direction), XMLoadFloat4x4(&camera->GetView()));
            XMStoreFloat4(&_psPerLightConstants.Direction, direction);

            _psPerLightConstants.Color = XMFLOAT4(directionalLight->Color.x, directionalLight->Color.y, directionalLight->Color.z, 1);

            XMStoreFloat4x4(&_psPerLightConstants.InvView, XMMatrixInverse(&det, XMLoadFloat4x4(&camera->GetView())));
            XMStoreFloat4x4(&_psPerLightConstants.LightView, XMLoadFloat4x4(&directionalLight->View));
            XMStoreFloat4x4(&_psPerLightConstants.LightProjection, XMLoadFloat4x4(&directionalLight->Projection));

            UpdateConstantBuffer(_context.Get(), _psPerLightConstantBuffer.Get(), _psPerLightConstants);

            ID3D11ShaderResourceView* shadow[] =
            {
                directionalLight->ShadowMap ? 
                    directionalLight->ShadowMap->GetShaderResourceView().Get() :
                    nullptr
            };
            _context->PSSetShaderResources(_countof(srvs), _countof(shadow), shadow);

            _context->DrawIndexed(_quad->GetIndexCount(), 0, 0);
        }
    }
    _cachedLights.clear();

    _context->OMSetBlendState(nullptr, blendAmounts, 0xFFFFFFFF);
}

PointLighting::PointLighting(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& depth, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& lights) :
    _context(context), _depth(depth), _normals(normals), _lights(lights)
{
    UNREFERENCED_PARAMETER(depth);
    UNREFERENCED_PARAMETER(normals);

    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsDirectionalLight.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psPointLight.cso");

    _vsPerFrameConstantBuffer = CreateConstantBuffer<vsPerFrameConstants>(device.Get());
    _vsPerObjectConstantBuffer = CreateConstantBuffer<vsPerObjectConstants>(device.Get());
    _psPerFrameConstantBuffer = CreateConstantBuffer<psPerFrameConstants>(device.Get());
    _psPerLightConstantBuffer = CreateConstantBuffer<psPerLightConstants>(device.Get());

    D3D11_TEXTURE2D_DESC td = {};
    lights->GetTexture()->GetDesc(&td);
    _psPerFrameConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    if (FAILED(device->CreateSamplerState(&sd, &_pointSampler)))
    {
        throw std::exception();
    }

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    if (FAILED(device->CreateSamplerState(&sd, &_linearSampler)))
    {
        throw std::exception();
    }

    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    if (FAILED(device->CreateBlendState(&bd, &_additiveBlend)))
    {
        throw std::exception();
    }

    _quad = Object::CreateQuad(device.Get(), 0, 0, 1, 1);

    ComPtr<IUnknown> unk;
    uint32_t unkSize = sizeof(IUnknown*);
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &unkSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> byteCode;
    unk.As(&byteCode);

    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(_quad->GetVertexFormat(), elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }
}

void PointLighting::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    XMVECTOR det;
    XMStoreFloat4x4(&_vsPerFrameConstants.InvProjection, XMMatrixInverse(&det, XMLoadFloat4x4(&camera->GetProjection())));
    UpdateConstantBuffer(_context.Get(), _vsPerFrameConstantBuffer.Get(), _vsPerFrameConstants);

    _psPerFrameConstants.ProjectionA = camera->GetProjection()._33;
    _psPerFrameConstants.ProjectionB = camera->GetProjection()._43;
    UpdateConstantBuffer(_context.Get(), _psPerFrameConstantBuffer.Get(), _psPerFrameConstants);

    float blendAmounts[] = { 1, 1, 1, 1 };
    _context->OMSetBlendState(_additiveBlend.Get(), blendAmounts, 0xFFFFFFFF);

    // Set buffer & nullptr for depth
    ID3D11RenderTargetView* rts[] = { _lights->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vsPerFrameConstantBuffer.GetAddressOf());

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    ID3D11Buffer* constantBuffers[] = { _psPerFrameConstantBuffer.Get(), _psPerLightConstantBuffer.Get() };
    _context->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

    // Set depth and normal maps as inputs
    ID3D11ShaderResourceView* srvs[] = { _depth->GetShaderResourceView().Get(), _normals->GetShaderResourceView().Get(), nullptr, nullptr };
    _context->PSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());
    _context->PSSetSamplers(1, 1, _linearSampler.GetAddressOf());

    uint32_t strides[] = { sizeof(VertexPositionNormal) };
    uint32_t offsets[] = { 0 };
    _context->IASetVertexBuffers(0, 1, _quad->GetVertexBuffer().GetAddressOf(), strides, offsets);
    _context->IASetIndexBuffer(_quad->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetInputLayout(_inputLayout.Get());

    XMStoreFloat4x4(&_psPerLightConstants.InvView, XMMatrixInverse(&det, XMLoadFloat4x4(&camera->GetView())));

    // Draw lights
    scene->GetLightsInSphere(XMFLOAT3(), 1, _cachedLights);
    for (auto light : _cachedLights)
    {
        if (light->Type == LightType::Point)
        {
            auto pointLight = reinterpret_cast<PointLight*>(light.get());

            _context->PSSetShaderResources(2, 1, pointLight->ShadowMapFront->GetShaderResourceView().GetAddressOf());
            _context->PSSetShaderResources(3, 1, pointLight->ShadowMapBack->GetShaderResourceView().GetAddressOf());

            XMVECTOR position = XMVector3Transform(XMLoadFloat3(&pointLight->Position), XMLoadFloat4x4(&camera->GetView()));
            XMStoreFloat4(&_psPerLightConstants.Position, position);
            _psPerLightConstants.Radius = pointLight->Radius;
            _psPerLightConstants.ParaboloidNear = 0.01f;

            _psPerLightConstants.Color = XMFLOAT4(pointLight->Color.x, pointLight->Color.y, pointLight->Color.z, 1);

            UpdateConstantBuffer(_context.Get(), _psPerLightConstantBuffer.Get(), _psPerLightConstants);

            _context->DrawIndexed(_quad->GetIndexCount(), 0, 0);
        }
    }
    _cachedLights.clear();

    _context->OMSetBlendState(nullptr, blendAmounts, 0xFFFFFFFF);
}
