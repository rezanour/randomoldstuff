#pragma once

struct MaterialSource
{
    char Diffuse[128];
    char Normal[128];
};

class TextureStreamer
{
public:
    TextureStreamer(_Inout_ std::unique_ptr<MaterialSource[]>& materials, uint32_t numMaterials, const ComPtr<ID3D11Texture2D>& textureArray, uint32_t initialMip);
    ~TextureStreamer();

    void RequestMaterial(uint32_t materialId, uint32_t index, _In_ uint32_t* mipBias);

private:
    struct Task
    {
        Task(uint32_t materialId, uint32_t index, uint32_t* mipBias) : MaterialId(materialId), Index(index), MipBias(mipBias) {}

        uint32_t    MaterialId;     // Material to load data for
        uint32_t    Index;          // Index in _textureArray where it should be loaded
        uint32_t*   MipBias;        // Pointer to value controlling starting mip for this texture. Set to 0 once task is done
    };

private:
    TextureStreamer(const TextureStreamer&);
    TextureStreamer& operator= (const TextureStreamer&);

    static DWORD CALLBACK StreamerThreadProc(_In_ void* context);
    void Run();
    void ProcessTask(_In_ const AssetLoader& assetLoader, _Inout_ Task& task);
    void Shutdown();

private:
    std::unique_ptr<MaterialSource[]> _materials;
    uint32_t _numMaterials;

    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11Texture2D> _textureArray;
    uint32_t _numMips;
    uint32_t _initialMip;

    // Texture2DArray with the number of elements equal to the total number
    // of materials. It is 1/4 width & height of the _textureArray, and
    // contains mips 2 -> n. When a material is to be paged in, we first
    // copy the mips 2 -> n from here to the new slot in _textureArray,
    // and then we start streaming in mip 1, then 0 (most detailed mip). Once
    // the mip has been streamed in, we can move the material's starting
    // mip value from 1 to 0 so that rendering can leverage the new detailed mip.
    ComPtr<ID3D11Texture2D> _staticMipChain;
    ComPtr<ID3D11Texture2D> _mip0Staging;
    ComPtr<ID3D11Texture2D> _mip1Staging;

    SRWLock _lock;
    HandleT<HandleTraits::HANDLENullTraits> _thread;
    Event _shutdown;
    Event _pending;
    Event _completed;
    std::deque<Task> _tasks;
};
