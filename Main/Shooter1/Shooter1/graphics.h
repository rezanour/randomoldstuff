#pragma once

class TexturePool;
class GeoPool;
class Texture;
class Geometry;
class Draw2D;
class Material;
enum class VertexFormat;

class Graphics : public TrackedObject<MemoryTag::Graphics>
{
public:
    std::shared_ptr<TexturePool> GetPoolWithSpace(_In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_ bool supportsMips, _In_ uint32_t numTextures);
    std::shared_ptr<GeoPool> GetPoolWithSpace(_In_ VertexFormat format, _In_ uint32_t numVertices, _In_ uint32_t numIndices);

    void Draw2D(_In_ const Texture& texture, _In_ const RECT& source, _In_ const RECT& dest, _In_ const XMFLOAT4& color = XMFLOAT4(1, 1, 1, 1));
    void Draw2DFullScreen(_In_ const Texture& texture, _In_ const RECT& source, _In_ const RECT& dest, _In_ const XMFLOAT4& color = XMFLOAT4(1, 1, 1, 1));

    void SetMaterial(_In_ MaterialType material);
    void Draw3D(_In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection, _In_count_(numObjects) GameObject** objects, _In_ uint32_t numObjects);

    void Present();

    //
    // TODO: Hide this later so that we don't leak DX constructs. Can't do it until we move Graphics impl
    // to code file and out of a header.
    //
    const ComPtr<ID3D11DeviceContext>& GetContext() const { return _context; }
    const ComPtr<ID3D11SamplerState>& GetPointWrapSampler() const { return _pointWrapSampler; }
    const ComPtr<ID3D11SamplerState>& GetLinearWrapSampler() const { return _linearWrapSampler; }

    SRWLock::SyncLockExclusive LockContext() { return _contextLock.LockExclusive(); }

private:
    friend void GraphicsStartup(_In_ void*);
    Graphics(_In_ void* window);
    Graphics(const Graphics&);

    void FindAdapterAndCreateDevice();
    void LoadMaterial(_In_ MaterialType material);
    void CreateStateObjects();

private:
    void* _window;
    ComPtr<IDXGIAdapter> _adapter;
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11RenderTargetView> _renderTarget;
    ComPtr<ID3D11DepthStencilView> _depthStencil;

    // State objects
    ComPtr<ID3D11SamplerState> _pointWrapSampler;
    ComPtr<ID3D11SamplerState> _linearWrapSampler;

    std::vector<std::shared_ptr<TexturePool>> _texturePools;
    std::vector<std::shared_ptr<GeoPool>> _geoPools;
    std::unique_ptr<::Draw2D> _draw2D;

    MaterialType _activeMaterial;
    std::unique_ptr<Material> _materials[(uint32_t)MaterialType::Max];

    SRWLock _contextLock;
};

void GraphicsStartup(_In_ void* window);
void GraphicsShutdown();

Graphics& GetGraphics();

//
// TODO: Move this! It's used to read in shader files
//
std::unique_ptr<uint8_t[]> ReadFile(_In_z_ const char* filename, _Out_ size_t* length);
