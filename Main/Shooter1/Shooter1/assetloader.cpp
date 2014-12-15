#include "precomp.h"
#include "assetloader.h"
#include "texture.h"
#include "bspcompiler.h"
#include "staticleveldata.h"
#include "texturestreamer.h"
#include "dds.h"

static AssetLoader* g_assetLoader;

AssetLoader::AssetLoader()
{
}

_Use_decl_annotations_
void AssetLoader::GetTextureMetadata(const char* filename, TextureMetadata* metadata) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    ImageMetadata data;
    DDSGetImageMetadata(filename, &data);

    ZeroMemory(metadata, sizeof(TextureMetadata));
    metadata->Width = data.Width;
    metadata->Height = data.Height;
    metadata->Format = data.Format;
    metadata->MipLevels = data.MipLevels;
}

_Use_decl_annotations_
void AssetLoader::LoadMips(const char* filename, uint32_t sourceStartMip, ID3D11Texture2D* dest, uint32_t destIndex, uint32_t destStartMip, uint32_t numMips) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    DDSLoadMipsIntoResource(source, sourceStartMip, numMips, dest, destIndex, destStartMip);
}

_Use_decl_annotations_
SingleSoundEffect AssetLoader::LoadSoundEffectFromFile(const char* filename) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    SoundFileData data;

    LoadSoundFileData(source, &data);

    // The DirectXTK Audio stack requires the memory that describes the WAV data to be
    // in a unique_ptr.

    /*
    struct SoundFileDataDeleter
    {
        void operator() (void* p)
        {
            SoundFileData data;
            data.Data = (uint8_t*)p; // Let the proper free occur
            data.DataSize = 0;
            data.Format = nullptr;
            data.StartAudioData = nullptr;
            data.StartAudioDataSize = 0;

            FreeSoundFileData(&data);
        }
    };
    */

    //std::unique_ptr<uint8_t[], SoundFileDataDeleter> wavData(data.Data);
    std::unique_ptr<uint8_t[]> wavData(data.Data);
    std::shared_ptr<SoundEffect> soundEffect(new SoundEffect(&GetGame().GetAudioEngine(), wavData, data.Format, data.StartAudioData, data.StartAudioDataSize));

    return SingleSoundEffect(soundEffect);
}

_Use_decl_annotations_
Texture AssetLoader::LoadTextureFromFile(const char* filename, bool supportsMips) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    ImageFileData data;
    LoadImageFileData(source, &data);

    Texture texture(LoadTextureFromMemory(data.Width, data.Height, data.Pitch, data.Data, data.Format, data.MipCount, supportsMips));

    FreeImageFileData(&data);

    return texture;
}

_Use_decl_annotations_
Texture AssetLoader::LoadTextureFromMemory(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* data, DXGI_FORMAT format, uint8_t mipCount, bool supportsMips) const
{
    if (mipCount > 1)
    {
        assert(supportsMips);
    }

    auto& graphics = GetGraphics();
    auto pool = graphics.GetPoolWithSpace(width, height, format, supportsMips, 1);

    uint32_t index = pool->ReserveRange(1);

    ComPtr<ID3D11Resource> resource;
    pool->Get()->GetResource(&resource);

    ComPtr<ID3D11Device> device;
    resource->GetDevice(&device);

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    pool->Get()->GetDesc(&desc);
    uint32_t numMips = std::min(desc.Texture2DArray.MipLevels, (UINT)mipCount);

    uint8_t* dataSrc = (uint8_t*)data;
    uint32_t dataHeight = height;
    uint32_t dataPitch = pitch;

    for (uint8_t mipIndex = 0; mipIndex < numMips; mipIndex++)
    {
        context->UpdateSubresource(resource.Get(), D3D11CalcSubresource(mipIndex, index, numMips), nullptr, dataSrc, dataPitch, static_cast<uint32_t>(dataPitch * dataHeight));
        dataSrc += static_cast<uint32_t>(dataPitch * dataHeight);
        dataHeight /= 2;
        dataPitch /= 2;
    }

    return Texture(pool, index);
}

_Use_decl_annotations_
void AssetLoader::LoadMipsIntoLocation(const char* filename, uint32_t sourceStartingMip, uint32_t numMips, const ComPtr<ID3D11Texture2D>& dest, uint32_t destIndex, uint32_t destStartingMip) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    ImageFileData data;
    LoadImageFileData(source, &data);

    ComPtr<ID3D11Device> device;
    dest->GetDevice(&device);

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    D3D11_TEXTURE2D_DESC desc;
    dest->GetDesc(&desc);

    assert(destStartingMip + numMips <= desc.MipLevels);
    assert(sourceStartingMip + numMips <= (uint32_t)data.MipCount);

    uint8_t* dataSrc = (uint8_t*)data.Data;
    uint32_t dataHeight = data.Height;
    uint32_t dataPitch = data.Pitch;

    // advance to source starting mip
    for (uint32_t i = 0; i < sourceStartingMip; ++i)
    {
        dataSrc += static_cast<uint32_t>(dataPitch * dataHeight);
        dataHeight /= 2;
        dataPitch /= 2;
    }

    auto lock = GetGraphics().LockContext();

    for (uint8_t i = 0; i < numMips; ++i)
    {
        context->UpdateSubresource(dest.Get(), D3D11CalcSubresource(destStartingMip + i, destIndex, numMips), nullptr, dataSrc, dataPitch, static_cast<uint32_t>(dataPitch * dataHeight));
        dataSrc += static_cast<uint32_t>(dataPitch * dataHeight);
        dataHeight /= 2;
        dataPitch /= 2;
    }
}

_Use_decl_annotations_
Geometry AssetLoader::LoadGeometryFromFile(const char* filename) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    GeometryFileData data;
    LoadGeometryFileData(source, &data);

    auto pool = GetGraphics().GetPoolWithSpace(VertexFormat::StaticGeometry, data.NumVertices, data.NumIndices);
    uint32_t verticesIndex = 0;
    uint32_t indicesIndex = 0;
    pool->ReserveRange(data.NumVertices, data.NumIndices, &verticesIndex, &indicesIndex);

    ComPtr<ID3D11Device> device;
    pool->GetVertices()->GetDevice(&device);

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    D3D11_BOX box = {};
    box.left = sizeof(StaticGeometryVertex) * verticesIndex;
    box.right = box.left + data.NumVertices * sizeof(StaticGeometryVertex);
    box.bottom = 1;
    box.back = 1;
    context->UpdateSubresource(pool->GetVertices(), 0, &box, data.Vertices, data.NumVertices * sizeof(StaticGeometryVertex), 0);

    box.left = sizeof(uint32_t) * indicesIndex;
    box.right = box.left + (data.NumIndices * sizeof(uint32_t));
    context->UpdateSubresource(pool->GetIndices(), 0, &box, data.Indices, data.NumIndices * sizeof(uint32_t), 0);

    Geometry geometry(pool, verticesIndex, indicesIndex, data.NumIndices);

    FreeGeometryFileData(&data);

    return geometry;
}

_Use_decl_annotations_
SpriteFont AssetLoader::LoadSpriteFontFromFile(const char* filename) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    SpriteFontFileData data;
    LoadSpriteFontFileData(source, &data);

    SpriteFont spriteFont(SpriteFont::Load(&data));

    FreeSpriteFontFileData(&data);

    return spriteFont;
}

_Use_decl_annotations_
StaticLevelData* AssetLoader::LoadStaticLevel(const char* filename) const
{
    char source[MAX_PATH];
    sprintf_s(source, "%s%s", GetConfig().ContentRoot, filename);

    // TODO: Replace this with real level data file

    GeometryFileData data;
    LoadGeometryFileData(source, &data);

    uint32_t numMaterials = data.NumMaterials;
    std::unique_ptr<MaterialSource[]> materials(new MaterialSource[numMaterials]);

    MaterialSource* material = materials.get();
    for (uint32_t i = 0; i < numMaterials; ++i, ++material)
    {
        strcpy_s(material->Diffuse, data.MaterialTextures[i]);
    }

    StaticLevelData* level = new StaticLevelData(GetGraphics().GetContext(), reinterpret_cast<StaticGeometryVertex*>(data.Vertices), data.NumVertices, data.Indices, data.NumIndices, materials, numMaterials, 256 * 1024 * 1024);

    FreeGeometryFileData(&data);

    return level;
}


void AssetLoaderStartup()
{
    assert(g_assetLoader == nullptr);

    CHECKHR(CoInitialize(NULL));

    delete g_assetLoader;

    g_assetLoader = new AssetLoader;

    DebugOut("AssetLoader initialized.\n");
}

void AssetLoaderShutdown()
{
    assert(g_assetLoader != nullptr);

    delete g_assetLoader;
    g_assetLoader = nullptr;

    CoUninitialize();

    DebugOut("AssetLoader shutdown.\n");
}

const AssetLoader& GetAssetLoader()
{
    assert(g_assetLoader != nullptr);
    return *g_assetLoader;
}
