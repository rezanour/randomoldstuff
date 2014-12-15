#include "DxGraphicsDevice.h"
#include "DxTexture.h"
#include "DxGeometry.h"

#include <GDKError.h>
#include <wrl\wrappers\corewrappers.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::FileHandle;

    std::shared_ptr<DxGraphicsDevice> DxGraphicsDevice::Create(_In_ const GraphicsDevice::CreateParameters& parameters)
    {
        // because some initialization can't be done via the constructor (requires shared_from_this), we do it in two steps
        auto device = std::shared_ptr<DxGraphicsDevice>(GDKNEW DxGraphicsDevice(parameters));
        device->ClearBoundResources();
        return device;
    }

    DxGraphicsDevice::DxGraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters) :
        GraphicsDevice(parameters)
    {
        DXGI_SWAP_CHAIN_DESC scd = {};

        scd.OutputWindow = static_cast<HWND>(_parameters.windowIdentity);
        scd.Windowed = true;
        scd.BufferCount = scd.Windowed ? 1 : 2;
        scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.Width = _parameters.backBufferWidth;
        scd.BufferDesc.Height = _parameters.backBufferHeight;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;

        static const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        D3D_FEATURE_LEVEL featureLevel;

        DWORD flags = 0;

    #if GDKDEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        CHECK_HR(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            featureLevels,
            _countof(featureLevels),
            D3D11_SDK_VERSION,
            &scd,
            &_swapChain,
            &_device,
            &featureLevel,
            &_context));

        ComPtr<ID3D11Texture2D> texture;

        // Get backbuffer
        CHECK_HR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture));
        CHECK_HR(_device->CreateRenderTargetView(texture.Get(), nullptr, &_backBufferView));

        // Setup the viewport
        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(_parameters.backBufferWidth),
        vp.Height = static_cast<float>(_parameters.backBufferHeight);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        _context->RSSetViewports(1, &vp);

        D3D11_TEXTURE2D_DESC texDesc = {};
        D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};

        // Create our depth & stencil buffer
        texDesc.Format = depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        texDesc.Width = _parameters.backBufferWidth;
        texDesc.Height = _parameters.backBufferHeight;
        texDesc.ArraySize = 1;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texDesc.MipLevels = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;

        depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        CHECK_HR(_device->CreateTexture2D(&texDesc, nullptr, texture.ReleaseAndGetAddressOf()));
        CHECK_HR(_device->CreateDepthStencilView(texture.Get(), &depthDesc, &_depthBufferView));

        // Load precompiled shaders
        FileHandle shaderFile(CreateFile(L"StaticGeometryVS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECK_WIN32(shaderFile.IsValid());

        BY_HANDLE_FILE_INFORMATION info = {0};
        CHECK_WIN32(GetFileInformationByHandle(shaderFile.Get(), &info));

        UINT fileLength = info.nFileSizeLow;
        std::unique_ptr<byte[]> shaderByteCode(new byte[fileLength]);
        DWORD bytesRead = 0;
        CHECK_WIN32(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr));

        CHECK_HR(_device->CreateVertexShader(shaderByteCode.get(), fileLength, nullptr, &_vertexShader));

        std::vector<D3D11_INPUT_ELEMENT_DESC> elems;

        D3D11_INPUT_ELEMENT_DESC elem = {0};
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

        CHECK_HR(_device->CreateInputLayout(elems.data(), static_cast<UINT>(elems.size()), shaderByteCode.get(), fileLength, &_inputLayout));

        shaderFile.Close();
        shaderFile.Attach(CreateFile(L"TexturedPS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECK_WIN32(shaderFile.IsValid());

        CHECK_WIN32(GetFileInformationByHandle(shaderFile.Get(), &info));

        fileLength = (static_cast<uint64_t>(info.nFileSizeHigh) << 32) | static_cast<uint64_t>(info.nFileSizeLow);
        shaderByteCode.reset(new byte[fileLength]);

        CHECK_WIN32(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr));

        CHECK_HR(_device->CreatePixelShader(shaderByteCode.get(), fileLength, nullptr, &_pixelShader));

        D3D11_BUFFER_DESC desc = {0};

        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = static_cast<UINT>(sizeof(_vsConstants));
        desc.StructureByteStride = static_cast<UINT>(sizeof(_vsConstants));
        desc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA init = {0};
        init.pSysMem = &_vsConstants;
        init.SysMemPitch = desc.ByteWidth;
        init.SysMemSlicePitch = 0;

        CHECK_HR(_device->CreateBuffer(&desc, &init, &_vsConstantBuffer));

        desc.ByteWidth = static_cast<UINT>(sizeof(_psConstants));
        desc.StructureByteStride = static_cast<UINT>(sizeof(_psConstants));

        init.pSysMem = &_psConstants;
        init.SysMemPitch = desc.ByteWidth;

        CHECK_HR(_device->CreateBuffer(&desc, &init, &_psConstantBuffer));

        _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
        _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
        _context->IASetInputLayout(_inputLayout.Get());
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->OMSetRenderTargets(1, _backBufferView.GetAddressOf(), _depthBufferView.Get());

        // Light1
        _psConstants.Light1.Direction = Vector4(0, 0, -1, 1);
        _psConstants.Light1.Color = Vector4(1, 1, 1, 1);

        // Light2
        _psConstants.Light2.Direction = Vector4(0, 1, 0, 1);
        _psConstants.Light2.Color = Vector4(0, 0, 1, 1);

        UpdatePSConstantBuffer();
    }

    void DxGraphicsDevice::Clear(_In_ const Vector4& color)
    {
        _context->ClearRenderTargetView(_backBufferView.Get(), reinterpret_cast<const float*>(&color.x));
    }

    void DxGraphicsDevice::ClearDepth(_In_ float depth)
    {
        _context->ClearDepthStencilView(_depthBufferView.Get(), D3D10_CLEAR_DEPTH, depth, 0);
    }

    void DxGraphicsDevice::Present()
    {
        _swapChain->Present(0, 0);
    }

    std::shared_ptr<Geometry> DxGraphicsDevice::CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data)
    {
        return DxGeometry::Create(shared_from_this(), data);
    }

    std::shared_ptr<Texture> DxGraphicsDevice::CreateTexture(_In_ const std::shared_ptr<TextureContent>& data)
    {
        return DxTexture::Create(shared_from_this(), data);
    }

    void DxGraphicsDevice::ClearBoundResources()
    {
        BindGeometry(nullptr);

        for (uint32_t i = 0; i < _countof(_boundTextures); ++i)
        {
            BindTexture(i, nullptr);
        }
    }

    void DxGraphicsDevice::BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture)
    {
        if (_boundTextures[slot] != nullptr)
        {
            _boundTextures[slot]->Unbind(slot);
        }

        if (texture != nullptr)
        {
            _boundTextures[slot] = texture;
        }
        else
        {
            if (_defaultTexture == nullptr)
            {
                CreateDefaultTexture();
            }
            _boundTextures[slot] = _defaultTexture;
        }

        if (_boundTextures[slot] != nullptr)
        {
            _boundTextures[slot]->Bind(slot);
        }
    }

    void DxGraphicsDevice::BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry)
    {
        if (_boundGeometry != nullptr)
        {
            _boundGeometry->Unbind();
        }

        _boundGeometry = geometry;

        if (_boundGeometry != nullptr)
        {
            _boundGeometry->Bind();
        }
    }

    void DxGraphicsDevice::SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection)
    {
        _vsConstants.ViewProj = view * projection;
        UpdateVSConstantBuffer();
    }

    void DxGraphicsDevice::Draw(_In_ const Matrix& world)
    {
        if (_boundGeometry)
        {
            _vsConstants.World = world;

            Matrix::Inverse(world, &_vsConstants.InvTransWorld);
            _vsConstants.InvTransWorld.Transpose();

            UpdateVSConstantBuffer();

            _boundGeometry->Draw(0);
        }
    }

    void DxGraphicsDevice::Draw2D(_In_ const RectangleF& destRect)
    {
        Vector2 size(destRect.width, destRect.height);
        Vector2 position(destRect.left + size.x * 0.5f, destRect.top + size.y * 0.5f);

        Matrix world = Matrix::CreateTranslation(Vector3(position.x - 0.5f, position.y - 0.5f, 0.0f)) * Matrix::CreateScale(Vector3(size.x, size.y, 1.0f));

        auto previousGeometry = _boundGeometry;
        auto previousViewProj = _vsConstants.ViewProj;

        SetViewProjection(Matrix::Identity(), Matrix::Identity());

        if (_quad == nullptr)
        {
            CreateQuad();
        }

        BindGeometry(_quad);
        Draw(world);

        BindGeometry(previousGeometry);

        _vsConstants.ViewProj = previousViewProj;
        UpdateVSConstantBuffer();
    }

    void DxGraphicsDevice::UpdateVSConstantBuffer()
    {
        _context->UpdateSubresource(_vsConstantBuffer.Get(), 0, nullptr, reinterpret_cast<const void*>(&_vsConstants), sizeof(_vsConstants), 0);
        _context->VSSetConstantBuffers(0, 1, _vsConstantBuffer.GetAddressOf());
    }

    void DxGraphicsDevice::UpdatePSConstantBuffer()
    {
        _context->UpdateSubresource(_psConstantBuffer.Get(), 0, nullptr, reinterpret_cast<const void*>(&_psConstants), sizeof(_psConstants), 0);
        _context->PSSetConstantBuffers(0, 1, _psConstantBuffer.GetAddressOf());
    }

    void DxGraphicsDevice::CreateDefaultTexture()
    {
        byte_t lightGreyPixel[] = { 190, 190, 190, 255 };
        _defaultTexture = DxTexture::Create(shared_from_this(), 1, 1, TextureFormat::R8G8B8A8, lightGreyPixel);
    }

    void DxGraphicsDevice::CreateQuad()
    {
        //// Create quad to use for 2D rendering
        Geometry::Vertex quadVertices[] =
        {
            { Vector3(-1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f) },
            { Vector3(1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f) },
            { Vector3(1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f) },
            { Vector3(-1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f) },
        };

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        _quad = DxGeometry::Create(shared_from_this(), 1, quadVertices, _countof(quadVertices), indices, _countof(indices));
    }
}
