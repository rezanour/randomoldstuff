#include "Precomp.h"
#include "VRES.h"

using namespace VRES;
using namespace OVR;

std::shared_ptr<HMD> HMD::Create(bool renderToHeadset)
{
    std::shared_ptr<HMD> hmd(new HMD(renderToHeadset));
    if (hmd != nullptr)
    {
        if (hmd->Initialize())
        {
            return hmd;
        }
    }

    return nullptr;
}

HMD::HMD(bool renderToHeadset) :
    _hmd(nullptr), _desc({0}), _renderToHeadset(renderToHeadset), _headtrackingEnabled(renderToHeadset)
{
}

HMD::~HMD()
{
    if (_hmd)
    {
        ovrHmd_Destroy(_hmd);
        _hmd = nullptr;
    }
}

bool HMD::Bind(Renderer* renderer)
{
    return GraphicsInit(renderer);
}

void HMD::GetWindowDimensions(_Out_ VRES::Point* location, _Out_ VRES::Size* size)
{
    location->X = _desc.WindowsPos.x;
    location->Y = _desc.WindowsPos.y;
    size->Width = _desc.Resolution.w;
    size->Height = _desc.Resolution.h;

    // If disconnected, show on main desktop. NOTE: this doesn't handle the case that it's plugged in, but off
    if (!_renderToHeadset || ((ovrHmd_GetSensorState(_hmd, 0.0f).StatusFlags & (ovrStatus_HmdConnected | ovrStatus_OrientationTracked)) == 0))
    {
        location->X = 50;
        location->Y = 50;
    }
}

XMFLOAT4 HMD::Orientation() const
{
    ovrQuatf orientation = ovrHmd_GetSensorState(_hmd, 0.0).Recorded.Pose.Orientation;
    return XMFLOAT4(orientation.x, orientation.y, -orientation.z, -orientation.w);
}

void HMD::SetCameraTransform(const XMFLOAT3& position, const XMFLOAT4& orientation)
{
    if (_renderToHeadset)
    {
        // Adjust the eyePose values by view
        for (uint32_t i = 0; i < _countof(_eyePose); ++i)
        {
            Matrix4f rotation;
            if (_headtrackingEnabled)
            {
                ovrQuatf eyeOrientation = _eyePose[i].Orientation;
                eyeOrientation.z *= -1;
                eyeOrientation.w *= -1;

                rotation = Matrix4f(Quatf(orientation.x, orientation.y, orientation.z, orientation.w)) * Matrix4f(eyeOrientation);
            }
            else
            {
                rotation = Matrix4f(Quatf(orientation.x, orientation.y, orientation.z, orientation.w));
            }

            Vector3f up = rotation.Transform(Vector3f(0, 1, 0));
            Vector3f forward = rotation.Transform(Vector3f(0, 0, 1));
            Vector3f shiftedEyePos = Vector3f(position.x, position.y, position.z) + rotation.Transform(_eyePose[i].Position);
            Matrix4f theView = Matrix4f::LookAtLH(shiftedEyePos,
                                                  shiftedEyePos + forward,
                                                  up);

            theView =  Matrix4f::Translation(_eyeRenderDesc[i].ViewAdjust) * theView;
            XMStoreFloat4x4(&_outputs[i].View, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theView.M)));

            Matrix4f theProj = ovrMatrix4f_Projection(_eyeRenderDesc[i].Fov, 0.01f, 10000.0f, false);
            XMStoreFloat4x4(&_outputs[i].Projection, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theProj.M)));

            _outputs[i].Viewport.TopLeftX = (float)_eyeRenderViewport[i].Pos.x;
            _outputs[i].Viewport.TopLeftY = (float)_eyeRenderViewport[i].Pos.y;
            _outputs[i].Viewport.Width = (float)_eyeRenderViewport[i].Size.w;
            _outputs[i].Viewport.Height = (float)_eyeRenderViewport[i].Size.h;
            _outputs[i].Viewport.MaxDepth = 1.0f;

            _outputs[i].CameraPosition = position;
        }
    }
    else
    {
        Matrix4f rotation = Matrix4f(Quatf(orientation.x, orientation.y, orientation.z, orientation.w));
        Vector3f up = rotation.Transform(Vector3f(0, 1, 0));
        Vector3f forward = rotation.Transform(Vector3f(0, 0, 1));
        Vector3f pos = Vector3f(position.x, position.y, position.z);
        Matrix4f theView = Matrix4f::LookAtLH(pos,
                                              pos + forward,
                                              up);

        XMStoreFloat4x4(&_outputs[0].View, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theView.M)));

        float radsH = XMConvertToRadians(80) * 0.5f;
        float radsV = XMConvertToRadians(60) * 0.5f;
        ovrFovPort fov = { tan(radsV), tan(radsV), tan(radsH), tan(radsH) };
        Matrix4f theProj = ovrMatrix4f_Projection(fov, 0.01f, 10000.0f, false);
        XMStoreFloat4x4(&_outputs[0].Projection, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theProj.M)));

        _outputs[0].Viewport.TopLeftX = 0.0f;
        _outputs[0].Viewport.TopLeftY = 0.0f;
        _outputs[0].Viewport.Width = (float)_desc.Resolution.w;
        _outputs[0].Viewport.Height = (float)_desc.Resolution.h;
        _outputs[0].Viewport.MaxDepth = 1.0f;

        _outputs[0].CameraPosition = position;
    }
}

void HMD::BeginFrame()
{
    if (!_renderToHeadset)
    {
        static const float clearColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
        _renderer->ClearBackBuffer(clearColor);
        _renderer->ClearDepth(1.0f);

        return;
    }

    ovrHmd_BeginFrame(_hmd, 0);

    ID3D11ShaderResourceView* empty[] = { nullptr, nullptr };
    _context->PSSetShaderResources(0, 2, empty);

    _context->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthStencil.Get());

    static const float clearColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
    _context->ClearRenderTargetView(_renderTarget.Get(), clearColor);
    _context->ClearDepthStencilView(_depthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    _eyePose[0] = ovrHmd_BeginEyeRender(_hmd, ovrEyeType::ovrEye_Left);
    _eyePose[1] = ovrHmd_BeginEyeRender(_hmd, ovrEyeType::ovrEye_Right);
}

void HMD::EndFrame()
{
    if (!_renderToHeadset)
    {
        _swapChain->Present(1, 0);
        return;
    }

    ovrHmd_EndEyeRender(_hmd, ovrEyeType::ovrEye_Left, _eyePose[0], &_eyeTexture[0].Texture);
    ovrHmd_EndEyeRender(_hmd, ovrEyeType::ovrEye_Right, _eyePose[1], &_eyeTexture[1].Texture);
    ovrHmd_EndFrame(_hmd);
}

bool HMD::Initialize()
{
    _hmd = ovrHmd_Create(0);
    if (!_hmd)
    {
        OutputDebugString(L"No HMD device detected, trying debug software device.\n");
        _hmd = ovrHmd_CreateDebug(ovrHmdType::ovrHmd_DK1);
        if (!_hmd) return false;
    }

    ovrHmd_GetDesc(_hmd, &_desc);

    OutputDebugString(L"Device Found: ");
    OutputDebugStringA(_desc.ProductName);
    OutputDebugString(L"\n");

    ovrHmd_SetEnabledCaps(_hmd, ovrHmdCap_LowPersistence);

    ovrHmd_StartSensor(_hmd, ovrSensorCap_Orientation |
                             ovrSensorCap_YawCorrection |
                             ovrSensorCap_Position, 0);

    return true;
}

bool HMD::GraphicsInit(Renderer* renderer)
{
    _renderer = renderer;
    _context = renderer->Context();
    _swapChain = renderer->SwapChain();

    if (!_renderToHeadset)
    {
        return true;
    }

    auto device = renderer->Device();

    // Configure stereo
    Sizei size0 = ovrHmd_GetFovTextureSize(_hmd, ovrEyeType::ovrEye_Left, _desc.DefaultEyeFov[0], 1.0f);
    Sizei size1 = ovrHmd_GetFovTextureSize(_hmd, ovrEyeType::ovrEye_Right, _desc.DefaultEyeFov[1], 1.0f);
    Sizei renderTargetSize(size0.w + size1.w, std::max(size0.h, size1.h));

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Height = renderTargetSize.h;
    td.Width = renderTargetSize.w;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr)) return false;

    hr = device->CreateRenderTargetView(texture.Get(), nullptr, &_renderTarget);
    if (FAILED(hr)) return false;

    hr = device->CreateShaderResourceView(texture.Get(), nullptr, &_shaderResource);
    if (FAILED(hr)) return false;

    ovrFovPort eyeFov[2] = { _desc.DefaultEyeFov[0], _desc.DefaultEyeFov[1] };

    _eyeRenderViewport[0].Pos = Vector2i(0, 0);
    _eyeRenderViewport[0].Size = Sizei(renderTargetSize.w / 2, renderTargetSize.h);
    _eyeRenderViewport[1].Pos = Vector2i((renderTargetSize.w + 1) / 2, 0);
    _eyeRenderViewport[1].Size = _eyeRenderViewport[0].Size;

    _eyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
    _eyeTexture[0].D3D11.Header.TextureSize = renderTargetSize;
    _eyeTexture[0].D3D11.Header.RenderViewport = _eyeRenderViewport[0];
    _eyeTexture[0].D3D11.pTexture = texture.Get();
    _eyeTexture[0].D3D11.pSRView = _shaderResource.Get();

    _eyeTexture[1] = _eyeTexture[0];
    _eyeTexture[1].D3D11.Header.RenderViewport = _eyeRenderViewport[1];

    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthTexture;
    hr = device->CreateTexture2D(&td, nullptr, &depthTexture);
    if (FAILED(hr)) return false;

    hr = device->CreateDepthStencilView(depthTexture.Get(), nullptr, &_depthStencil);
    if (FAILED(hr)) return false;

    renderer->Context()->OMGetRenderTargets(1, &_prevRTV, &_prevDSV);

    ovrD3D11Config d3d11Config;
    d3d11Config.D3D11.Header.API = ovrRenderAPI_D3D11;
    d3d11Config.D3D11.Header.RTSize = _desc.Resolution;
    d3d11Config.D3D11.Header.Multisample = 1;
    d3d11Config.D3D11.pDevice = device.Get();
    d3d11Config.D3D11.pDeviceContext = renderer->Context().Get();
    d3d11Config.D3D11.pSwapChain = renderer->SwapChain().Get();
    d3d11Config.D3D11.pBackBufferRT = _prevRTV.Get();

    return !!ovrHmd_ConfigureRendering(_hmd, &d3d11Config.Config,
                                       ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
                                       eyeFov, _eyeRenderDesc);
}
