#include "Dx11GraphicsDevice.h"
#include <wrl\wrappers\corewrappers.h>

namespace GDK
{
    // REMOVE THESE AS SOON AS WE HAVE IMPLS
    class Texture
    {
    };

    class Geometry
    {
    };

    // TODO: Move these out to a shared *internal* location for com code?

#define CHECKHR(x) { HRESULT hr = (x); if (FAILED(hr)) throw GDKException(#x, static_cast<int32_t>(hr)); }
#define CHECKGLE(x) if (!(x)) throw GDKException(#x, GetLastError());
    namespace Graphics
    {
        using Microsoft::WRL::Wrappers::FileHandle;

        Dx11GraphicsDevice::Dx11GraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters) :
            GraphicsDevice(parameters)
        {
            DXGI_SWAP_CHAIN_DESC scd = {0};

            scd.OutputWindow = static_cast<HWND>(parameters.WindowIdentity);
            scd.Windowed = FALSE;
            scd.BufferCount = scd.Windowed ? 1 : 2;
            scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            scd.BufferDesc.Width = parameters.BackBufferWidth;
            scd.BufferDesc.Height = parameters.BackBufferHeight;
            scd.BufferDesc.RefreshRate.Numerator = 60;
            scd.BufferDesc.RefreshRate.Denominator = 1;
            scd.SampleDesc.Count = 1;
            scd.SampleDesc.Quality = 0;

            D3D_FEATURE_LEVEL featureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
            };

            D3D_FEATURE_LEVEL featureLevel;

            DWORD flags = 0;

        #ifndef NDEBUG
            flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

            CHECKHR(D3D11CreateDeviceAndSwapChain(
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
            CHECKHR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture));
            CHECKHR(_device->CreateRenderTargetView(texture.Get(), nullptr, &_backBufferView));

            // Setup the viewport
            D3D11_VIEWPORT vp;
            vp.Width = static_cast<float>(parameters.BackBufferWidth);
            vp.Height = static_cast<float>(parameters.BackBufferHeight);
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;

            _context->RSSetViewports(1, &vp);

            D3D11_TEXTURE2D_DESC texDesc = {};
            D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};

            // Create our depth & stencil buffer
            texDesc.Format = depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            texDesc.Width = parameters.BackBufferWidth;
            texDesc.Height = parameters.BackBufferHeight;
            texDesc.ArraySize = 1;
            texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            texDesc.MipLevels = 1;
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;

            depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            CHECKHR(_device->CreateTexture2D(&texDesc, nullptr, texture.ReleaseAndGetAddressOf()));
            CHECKHR(_device->CreateDepthStencilView(texture.Get(), &depthDesc, &_depthBufferView));

            // Load precompiled shaders
            FileHandle shaderFile(CreateFile(L"StaticGeometryVS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            CHECKGLE(shaderFile.IsValid());

            BY_HANDLE_FILE_INFORMATION info = {0};
            CHECKGLE(GetFileInformationByHandle(shaderFile.Get(), &info));

            UINT fileLength = info.nFileSizeLow;
            std::unique_ptr<byte[]> shaderByteCode(new byte[fileLength]);
            DWORD bytesRead = 0;
            CHECKGLE(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr));

            CHECKHR(_device->CreateVertexShader(shaderByteCode.get(), fileLength, nullptr, &_vertexShader));

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

            CHECKHR(_device->CreateInputLayout(elems.data(), static_cast<UINT>(elems.size()), shaderByteCode.get(), fileLength, &_inputLayout));

            shaderFile.Close();
            shaderFile.Attach(CreateFile(L"TexturedPS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            CHECKGLE(shaderFile.IsValid());

            CHECKGLE(GetFileInformationByHandle(shaderFile.Get(), &info));

            fileLength = (static_cast<uint64_t>(info.nFileSizeHigh) << 32) | static_cast<uint64_t>(info.nFileSizeLow);
            shaderByteCode.reset(new byte[fileLength]);

            CHECKGLE(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr));

            CHECKHR(_device->CreatePixelShader(shaderByteCode.get(), fileLength, nullptr, &_pixelShader));

            D3D11_BUFFER_DESC desc = {0};

            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.ByteWidth = static_cast<UINT>(sizeof(_vsConstants));
            desc.StructureByteStride = static_cast<UINT>(sizeof(_vsConstants));
            desc.Usage = D3D11_USAGE_DEFAULT;

            D3D11_SUBRESOURCE_DATA init = {0};
            init.pSysMem = &_vsConstants;
            init.SysMemPitch = desc.ByteWidth;
            init.SysMemSlicePitch = 0;

            CHECKHR(_device->CreateBuffer(&desc, &init, &_vsConstantBuffer));

            desc.ByteWidth = static_cast<UINT>(sizeof(_psConstants));
            desc.StructureByteStride = static_cast<UINT>(sizeof(_psConstants));

            init.pSysMem = &_psConstants;
            init.SysMemPitch = desc.ByteWidth;

            CHECKHR(_device->CreateBuffer(&desc, &init, &_psConstantBuffer));

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

            //UpdatePSConstantBuffer();

            //// Create default texture for untextured models
            //byte_t lightGreyPixel[] = { 190, 190, 190, 255 };
            //_defaultTexture = Texture::CreateFromData(this, 1, 1, lightGreyPixel);

            //// Create quad to use for 2D rendering
            //Geometry::Vertex quadVertices[] =
            //{
            //    { Vector3(-1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f) },
            //    { Vector3(1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f) },
            //    { Vector3(1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f) },
            //    { Vector3(-1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f) },
            //};

            //uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

            //_quad = Geometry::CreateFromData(device.Get(), 1, _countof(quadVertices), quadVertices, _countof(indices), indices);
        }

        void Dx11GraphicsDevice::Clear(_In_ const Vector4& color)
        {
            _context->ClearRenderTargetView(_backBufferView.Get(), reinterpret_cast<const float*>(&color.x));
        }

        void Dx11GraphicsDevice::ClearDepth(_In_ float depth)
        {
            _context->ClearDepthStencilView(_depthBufferView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
        }

        void Dx11GraphicsDevice::Present()
        {
            _swapChain->Present(0, 0);
        }

        std::shared_ptr<Geometry>  Dx11GraphicsDevice::CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data)
        {
            UNREFERENCED_PARAMETER(data);
            return nullptr;
        }

        std::shared_ptr<Texture>  Dx11GraphicsDevice::CreateTexture(_In_ const std::shared_ptr<TextureContent>& data)
        {
            UNREFERENCED_PARAMETER(data);
            return nullptr;
        }

        void Dx11GraphicsDevice::ClearBoundResources()
        {
        }

        void Dx11GraphicsDevice::BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry)
        {
            UNREFERENCED_PARAMETER(geometry);
        }

        void Dx11GraphicsDevice::BindTexture(_In_ size_t slot, _In_ const std::shared_ptr<Texture>& texture)
        {
            UNREFERENCED_PARAMETER(slot);
            UNREFERENCED_PARAMETER(texture);
        }

        void Dx11GraphicsDevice::SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection)
        {
            UNREFERENCED_PARAMETER(view);
            UNREFERENCED_PARAMETER(projection);
        }

        void Dx11GraphicsDevice::Draw(_In_ const Matrix& world)
        {
            UNREFERENCED_PARAMETER(world);
        }

        void Dx11GraphicsDevice::Draw2D(_In_ size_t slot, _In_ const RectangleF& destRect)
        {
            UNREFERENCED_PARAMETER(slot);
            UNREFERENCED_PARAMETER(destRect);
        }
    }
}
