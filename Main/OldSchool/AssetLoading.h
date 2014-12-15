#pragma once

struct IGraphicsSystem;
struct ITexture;
struct IGeometry;
struct IAudioSystem;
struct ISound;
struct ISpriteFont;
struct IGraphicsScene;
struct PositionNormalTangentTextureVertex;

typedef bool(*VerticesLoaded)(_In_ void* callerContext, _In_ const PositionNormalTangentTextureVertex* vertices, _In_ const uint32_t numVertices, _In_ const uint32_t* indices, _In_ const uint32_t numIndices, _In_ const Transform& transform);

class AssetLoader : public BaseObject<AssetLoader>
{
public:
    static std::shared_ptr<AssetLoader> Create();

    std::shared_ptr<ITexture> LoadTextureFromFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename, _In_ bool generateMips, _Out_opt_ std::shared_ptr<ITexture>* normalMap);
    std::shared_ptr<ITexture> LoadTextureFromGDKTextureFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename, _In_ bool generateMips, _Out_opt_ std::shared_ptr<ITexture>* normalMap);
    std::shared_ptr<ISound> LoadSoundFromFile(_In_ const std::shared_ptr<IAudioSystem>& audio, _In_z_ const wchar_t* filename);
    std::shared_ptr<ISpriteFont> LoadSpriteFontFromFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename);
    std::shared_ptr<IGraphicsScene> LoadSceneFromFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename);
    std::shared_ptr<IGeometry> LoadGeometryFromOBJFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename, _In_ const Transform& transform);
    std::shared_ptr<IGeometry> LoadGeometryFromGDKGeometryFile(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* filename, _In_ float targetHeight, _In_ const Transform& transform);

    void SetVerticesLoadedCallback(_In_ void* callerContext, _In_ VerticesLoaded callback);

    static void ReadFileToBuffer(_In_ const wchar_t* path, _Inout_ std::vector<uint8_t>& buffer);

private:
    AssetLoader();

    std::shared_ptr<ITexture> GenerateNormalMapFromTexture(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_ const void* pixels, _In_ float amplitude);

private:
    ComPtr<IWICImagingFactory> _wicFactory;
    VerticesLoaded _verticesLoadedCallback;
    void* _verticesLoadedCallbackCallerContext;
};

