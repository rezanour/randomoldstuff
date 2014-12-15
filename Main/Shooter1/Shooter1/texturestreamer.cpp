#include "precomp.h"
#include "texturestreamer.h"
#include <DirectXTex.h>

_Use_decl_annotations_
TextureStreamer::TextureStreamer(std::unique_ptr<MaterialSource[]>& materials, uint32_t numMaterials, const ComPtr<ID3D11Texture2D>& textureArray, uint32_t initialMip) :
    _numMaterials(numMaterials), _textureArray(textureArray), _initialMip(initialMip)
{
    _materials.swap(materials);

    ComPtr<ID3D11Device> device;
    textureArray->GetDevice(&device);

    device->GetImmediateContext(&_context);

    D3D11_TEXTURE2D_DESC td;
    textureArray->GetDesc(&td);

    _numMips = td.MipLevels - 2;

    td.MipLevels = 1;
    td.ArraySize = 1;
    td.BindFlags = 0;
    td.Usage = D3D11_USAGE_STAGING;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    td.MiscFlags = 0;

    CHECKHR(device->CreateTexture2D(&td, nullptr, &_mip0Staging));

    td.Width /= 2;
    td.Height /= 2;
    CHECKHR(device->CreateTexture2D(&td, nullptr, &_mip1Staging));

    ComPtr<ID3D11Texture2D> loadStaging;
    td.Width /= 2;
    td.Height /= 2;
    td.MipLevels = 0;
    CHECKHR(device->CreateTexture2D(&td, nullptr, &loadStaging));

    td.ArraySize = _numMaterials;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.CPUAccessFlags = 0;
    CHECKHR(device->CreateTexture2D(&td, nullptr, &_staticMipChain));

    // Load in all low resolution mips
    auto& assetLoader = GetAssetLoader();
    MaterialSource* material = _materials.get();
    for (uint32_t i = 0; i < numMaterials; ++i, ++material)
    {
        assetLoader.LoadMips(material->Diffuse, _initialMip + 2, loadStaging.Get(), 0, 0, _numMips);

        for (uint32_t j = 0; j < _numMips; ++j)
        {
            _context->CopySubresourceRegion(_staticMipChain.Get(), D3D11CalcSubresource(j, i, _numMips), 0, 0, 0, loadStaging.Get(), j, nullptr);
        }
    }

    _pending.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    CHECKTRUE(_pending.IsValid());

    _shutdown.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    CHECKTRUE(_shutdown.IsValid());

    _completed.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    CHECKTRUE(_completed.IsValid());

    _thread.Attach(CreateThread(nullptr, 0, StreamerThreadProc, this, 0, nullptr));
    CHECKTRUE(_thread.IsValid());
}

TextureStreamer::~TextureStreamer()
{
    if (_thread.IsValid())
    {
        Shutdown();
        WaitForSingleObject(_thread.Get(), INFINITE);
    }
}

_Use_decl_annotations_
void TextureStreamer::RequestMaterial(uint32_t materialId, uint32_t index, uint32_t* mipBias)
{
    // Set the starting mip to 2 until we can stream in 1 & 0
    *mipBias = 250;

    // Synchronously copy in lower mips
    for (uint32_t i = 0; i < _numMips; ++i)
    {
        _context->CopySubresourceRegion(_textureArray.Get(), D3D11CalcSubresource(i + 2, index, _numMips + 2), 0, 0, 0,
                                        _staticMipChain.Get(), D3D11CalcSubresource(i, materialId, _numMips), nullptr);
    }

    // Queue work to stream in mip 0 and 1
    {
        auto lock = _lock.LockExclusive();
        _tasks.emplace_back(Task(materialId, index, mipBias)); // one for mip 1
        _tasks.emplace_back(Task(materialId, index, mipBias)); // one for mip 2
    }

    // Notify worker thread
    SetEvent(_pending.Get());
}

_Use_decl_annotations_
DWORD CALLBACK TextureStreamer::StreamerThreadProc(void* context)
{
    TextureStreamer* streamer = static_cast<TextureStreamer*>(context);
    streamer->Run();

    return 0;
}

void TextureStreamer::Run()
{
    HANDLE events[] = { _pending.Get(), _shutdown.Get() };
    std::deque<Task> tasks;
    auto& assetLoader = GetAssetLoader();
    bool shuttingDown = false;

    while (!shuttingDown)
    {
        DWORD ret = WaitForMultipleObjectsEx(_countof(events), events, FALSE, INFINITE, FALSE);
        shuttingDown = (ret == (WAIT_OBJECT_0 + 1));

        {
            auto lock = _lock.LockExclusive();
            tasks.swap(_tasks);
        }

        for (auto& task : tasks)
        {
            Sleep(250);
            ProcessTask(assetLoader, task);
        }

        tasks.clear();
    }
}

_Use_decl_annotations_
void TextureStreamer::ProcessTask(const AssetLoader& assetLoader, Task& task)
{
    auto& material = _materials.get()[task.MaterialId];
    auto& animations = GetAnimations();

    auto lock = GetGraphics().LockContext();

    if (*task.MipBias == 250) // loading mip 1
    {
        assetLoader.LoadMips(material.Diffuse, _initialMip + 1, _mip1Staging.Get(), 0, 0, 1);
        _context->CopySubresourceRegion(_textureArray.Get(), D3D11CalcSubresource(1, task.Index, _numMips + 2), 0, 0, 0, _mip1Staging.Get(), 0, nullptr);
        uint32_t* mipBias = task.MipBias;
        animations.CreateAnimation((float)*task.MipBias, 150.0f, 1.0f, false, [mipBias](float value) { *mipBias = (uint32_t)value; }, true);
    }
    else
    {
        assetLoader.LoadMips(material.Diffuse, _initialMip, _mip0Staging.Get(), 0, 0, 1);
        _context->CopySubresourceRegion(_textureArray.Get(), D3D11CalcSubresource(0, task.Index, _numMips + 2), 0, 0, 0, _mip0Staging.Get(), 0, nullptr);
        uint32_t* mipBias = task.MipBias;
        animations.CreateAnimation((float)*task.MipBias, 0.0f, 1.0f, false, [mipBias](float value) { *mipBias = (uint32_t)value; }, true);
    }
}

void TextureStreamer::Shutdown()
{
    SetEvent(_shutdown.Get());
}

