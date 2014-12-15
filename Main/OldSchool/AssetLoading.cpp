#include "Precomp.h"
#include <filesystem>
#include <fstream>
#include <DirectXTex.h>

#include "AssetLoading.h"
#include "GraphicsSystem.h"
#include "Texture.h"
#include "Geometry.h"
#include "Model.h"
#include "Light.h"
#include "GraphicsScene.h"
#include "AudioSystem.h"
#include "Sound.h"
#include "SpriteFont.h"
#include "Transform.h"

std::shared_ptr<AssetLoader> AssetLoader::Create()
{
    return std::shared_ptr<AssetLoader>(new AssetLoader);
}

AssetLoader::AssetLoader() :
    _verticesLoadedCallback(nullptr),
    _verticesLoadedCallbackCallerContext(nullptr)
{
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_wicFactory));
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

_Use_decl_annotations_
std::shared_ptr<ITexture> AssetLoader::LoadTextureFromFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename, bool generateMips, std::shared_ptr<ITexture>* normalMap)
{
    std::wstring path = filename;
    if (path.rfind(L".texture") != std::wstring::npos)
    {
        return LoadTextureFromGDKTextureFile(graphics, filename, generateMips, normalMap);
    }

    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = _wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    ComPtr<IWICFormatConverter> converter;
    hr = _wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    uint32_t width, height;
    hr = frame->GetSize(&width, &height);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[width * height * sizeof(uint32_t)]);
    hr = converter->CopyPixels(nullptr, width * sizeof(uint32_t), width * height * sizeof(uint32_t), pixels.get());
    if (FAILED(hr))
    {
        throw std::exception();
    }

    if (normalMap != nullptr)
    {
        *normalMap = GenerateNormalMapFromTexture(graphics, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, pixels.get(), 2.0f);
    }

    return graphics->CreateTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, pixels.get(), width * sizeof(uint32_t), generateMips);
}

_Use_decl_annotations_
std::shared_ptr<ITexture> AssetLoader::LoadTextureFromGDKTextureFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename, bool generateMips, std::shared_ptr<ITexture>* normalMap)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);

    const uint32_t TextureContentVersion = 0x20315254; // "TR1 "

#pragma pack(push,1)

    enum class TextureFormat
    {
        R8G8B8A8,
    };

    struct TextureContentHeader
    {
        uint32_t        Version;
        uint32_t        Id;
        TextureFormat   Format;
        uint32_t        Width;
        uint32_t        Height;
        uint32_t        NumFrames;
        uint32_t        ImageSize;
    };

#pragma pack(pop)

    uint8_t* buf = buffer.data();
    TextureContentHeader* header = (TextureContentHeader*)buf;
    buf += sizeof(TextureContentHeader);

    if (header->Version != TextureContentVersion)
    {
        throw new std::exception();
    }

    if (normalMap != nullptr)
    {
        *normalMap = GenerateNormalMapFromTexture(graphics, header->Width, header->Height, DXGI_FORMAT_R8G8B8A8_UNORM, buf, 2.0f);
    }

    return graphics->CreateTexture(header->Width, header->Height, DXGI_FORMAT_R8G8B8A8_UNORM, buf, header->Width * sizeof(uint32_t), generateMips);
}

_Use_decl_annotations_
std::shared_ptr<ISound> AssetLoader::LoadSoundFromFile(const std::shared_ptr<IAudioSystem>& audio, const wchar_t* filename)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);

    return audio->CreateSound(buffer.data(), (uint32_t)buffer.size());
}

_Use_decl_annotations_
std::shared_ptr<ISpriteFont> AssetLoader::LoadSpriteFontFromFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);

    return CreateSpriteFont(graphics, (uint32_t)buffer.size(), buffer.data());
}

_Use_decl_annotations_
std::shared_ptr<IGraphicsScene> AssetLoader::LoadSceneFromFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);
    std::shared_ptr<IGraphicsScene> scene;
    std::shared_ptr<IGeometry> wall;

    static std::vector<std::shared_ptr<ReadonlyObject>> s_statics;

    // Plane geometry
    PositionNormalTangentTextureVertex vertices[] = {
        { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
    };

    uint32_t indices[] =
    {
        0, 1, 2, 0, 2, 3
    };

    // Create resuable wall geometry
    wall = graphics->CreateGeometry(_countof(vertices), VertexFormat::PositionNormalTangentTexture, vertices, _countof(indices), indices);

    // Create scene
    scene = graphics->CreateScene();

    char* text = (char*)buffer.data();
    char* ctx = nullptr;
    char * line = nullptr;
    line = strtok_s(text, "\r\n", &ctx);
    while (line != nullptr)
    {
        char objType[32] = { 0 };
        if (line && line[0] != 0 && line[0] != '\n' && line[0] != '/')
        {
            sscanf_s(line, "%s", objType, (uint32_t)_countof(objType));
        }

        if (strlen(objType))
        {
            if (_strcmpi(objType, "alight") == 0)
            {
                std::shared_ptr<ILight> light;
                XMFLOAT3 clr;
                sscanf_s(line, "%s %f %f %f ", objType, (uint32_t)_countof(objType), &clr.x, &clr.y, &clr.z);
                light = graphics->CreateLight(LightType::Ambient, clr, 0.0f);
                scene->AddLight(light);
            }
            else if (_strcmpi(objType, "dlight") == 0)
            {
                std::shared_ptr<ILight> light;
                XMFLOAT3 pos;
                XMFLOAT3 clr;
                sscanf_s(line, "%s %f %f %f %f %f %f ", objType, (uint32_t)_countof(objType), &pos.x, &pos.y, &pos.z, &clr.x, &clr.y, &clr.z);
                light = graphics->CreateLight(LightType::Directional, clr, 0.0f);
                light->SetPosition(pos);
                scene->AddLight(light);
            }
            else if (_strcmpi(objType, "plight") == 0)
            {
                std::shared_ptr<ILight> light;
                XMFLOAT3 pos;
                XMFLOAT3 clr;
                float radius;
                sscanf_s(line, "%s %f %f %f %f %f %f %f ", objType, (uint32_t)_countof(objType), &pos.x, &pos.y, &pos.z, &radius, &clr.x, &clr.y, &clr.z);
                light = graphics->CreateLight(LightType::Point, clr, radius);
                light->SetPosition(pos);
                scene->AddLight(light);
            }
            else if (_strcmpi(objType, "wall") == 0)
            {
                std::shared_ptr<IModel> model;
                Vector3 pos;
                Vector3 n;
                char textureName[32] = { 0 };
                char normalTextureName[32] = { 0 };
                char specularTextureName[32] = { 0 };
                float width = 1;
                float height = 1;
                sscanf_s(line, "%s %f %f %f %f %f %f %f %f %s %s %s", objType, (uint32_t)_countof(objType), &pos.x, &pos.y, &pos.z, &width, &height, &n.x, &n.y, &n.z, textureName, (uint32_t)_countof(textureName), normalTextureName, (uint32_t)_countof(normalTextureName), specularTextureName, (uint32_t)_countof(specularTextureName));

                std::shared_ptr<ITexture> texture;
                std::shared_ptr<ITexture> normTex;
                if (strlen(normalTextureName) == 0)
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, &normTex);
                }
                else
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, nullptr);
                    normTex = LoadTextureFromFile(graphics, std::AnsiToWide(normalTextureName).c_str(), false, nullptr);
                }

                std::shared_ptr<ITexture> specTex;
                if (strlen(specularTextureName) > 0)
                {
                    specTex = LoadTextureFromFile(graphics, std::AnsiToWide(specularTextureName).c_str(), false, nullptr);
                }

                Transform transform(pos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(width, height, 1));
                transform.LookAt(pos + n, Vector3(0, 1, 0));

                auto staticObj = ReadonlyObject::Create(transform, AABB());
                s_statics.push_back(staticObj);

                model = graphics->CreateModel(staticObj);
                model->SetGeometry(wall);
                model->SetTexture(TextureUsage::Diffuse, texture);
                model->SetTexture(TextureUsage::Normal, normTex);
                model->SetTexture(TextureUsage::Specular, specTex);

                scene->AddModel(model);
            }
            else if (_strcmpi(objType, "obj") == 0)
            {
                std::shared_ptr<IModel> model;
                std::shared_ptr<IGeometry> geo;
                Vector3 pos;
                Vector3 scale;
                Vector3 n;
                char textureName[32] = { 0 };
                char normalTextureName[32] = { 0 };
                char specularTextureName[32] = { 0 };
                char objName[32] = { 0 };
                sscanf_s(line, "%s %f %f %f %f %f %f %f %f %f %s %s %s %s", objType, (uint32_t)_countof(objType), &pos.x, &pos.y, &pos.z, &scale.x, &scale.y, &scale.z, &n.x, &n.y, &n.z, objName, (uint32_t)_countof(objName), textureName, (uint32_t)_countof(textureName), normalTextureName, (uint32_t)_countof(normalTextureName), specularTextureName, (uint32_t)_countof(specularTextureName));

                std::shared_ptr<ITexture> texture;
                std::shared_ptr<ITexture> normTex;
                if (strlen(normalTextureName) == 0)
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, &normTex);
                }
                else
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, nullptr);
                    normTex = LoadTextureFromFile(graphics, std::AnsiToWide(normalTextureName).c_str(), false, nullptr);
                }

                std::shared_ptr<ITexture> specTex;
                if (strlen(specularTextureName) > 0)
                {
                    specTex = LoadTextureFromFile(graphics, std::AnsiToWide(specularTextureName).c_str(), false, nullptr);
                }

                Transform transform(pos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(scale.x, scale.y, scale.z));
                transform.LookAt(pos + n, Vector3(0, 1, 0));

                geo = LoadGeometryFromOBJFile(graphics, std::AnsiToWide(objName).c_str(), transform);

                auto staticObj = ReadonlyObject::Create(transform, AABB());
                s_statics.push_back(staticObj);

                model = graphics->CreateModel(staticObj);
                model->SetGeometry(geo);
                model->SetTexture(TextureUsage::Diffuse, texture);
                model->SetTexture(TextureUsage::Normal, normTex);
                model->SetTexture(TextureUsage::Specular, specTex);

                scene->AddModel(model);
            }
            else if (_strcmpi(objType, "gdkgeo") == 0)
            {
                std::shared_ptr<IModel> model;
                std::shared_ptr<IGeometry> geo;
                Vector3 pos;
                Vector3 scale;
                Vector3 n;
                char textureName[32] = { 0 };
                char normalTextureName[32] = { 0 };
                char specularTextureName[32] = { 0 };
                char objName[32] = { 0 };
                float targetHeight = 1.0f;
                sscanf_s(line, "%s %f %f %f %f %f %f %f %f %f %s %s %f %s %s", objType, (uint32_t)_countof(objType), &pos.x, &pos.y, &pos.z, &scale.x, &scale.y, &scale.z, &n.x, &n.y, &n.z, objName, (uint32_t)_countof(objName), textureName, (uint32_t)_countof(textureName), &targetHeight, normalTextureName, (uint32_t)_countof(normalTextureName), specularTextureName, (uint32_t)_countof(specularTextureName));

                std::shared_ptr<ITexture> texture;
                std::shared_ptr<ITexture> normTex;
                if (strlen(normalTextureName) == 0)
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, &normTex);
                }
                else
                {
                    texture = LoadTextureFromFile(graphics, std::AnsiToWide(textureName).c_str(), true, nullptr);
                    normTex = LoadTextureFromFile(graphics, std::AnsiToWide(normalTextureName).c_str(), false, nullptr);
                }

                std::shared_ptr<ITexture> specTex;
                if (strlen(specularTextureName) > 0)
                {
                    specTex = LoadTextureFromFile(graphics, std::AnsiToWide(specularTextureName).c_str(), false, nullptr);
                }

                Transform transform(pos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(scale.x, scale.y, scale.z));

                geo = LoadGeometryFromGDKGeometryFile(graphics, std::AnsiToWide(objName).c_str(), targetHeight, transform);

                auto staticObj = ReadonlyObject::Create(transform, AABB());
                s_statics.push_back(staticObj);

                model = graphics->CreateModel(staticObj);
                model->SetGeometry(geo);
                model->SetTexture(TextureUsage::Diffuse, texture);
                model->SetTexture(TextureUsage::Normal, normTex);
                model->SetTexture(TextureUsage::Specular, specTex);

                scene->AddModel(model);
            }
        }

        line = strtok_s(nullptr, "\r\n", &ctx);
    }

    return scene;
}

// This loader only loads the first frame of the geometry.  So if a model has more than one animation frame, only
// the first one is used.
_Use_decl_annotations_
std::shared_ptr<IGeometry> AssetLoader::LoadGeometryFromGDKGeometryFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename, float targetHeight, const Transform& transform)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);

    const uint32_t GeometryContentVersion = 0x20315247; // "GR1 "

#pragma pack(push,1)
    struct GeometryContentHeader
    {
        uint32_t Version;
        uint32_t Id;
        uint32_t AttributeCount;
        uint32_t VertexStride;
        uint32_t VertexCount;
        uint32_t IndexCount;
        uint32_t FrameCount;
        uint32_t AnimationCount;
    };

    enum class AttributeName
    {
        None,
        Position,
        Normal,
        TexCoord
    };

    enum class AttributeType
    {
        Null,
        Float2,
        Float3
    };

    struct AttributeDesc
    {
        uint8_t offset;
        AttributeType type;
        AttributeName semanticName;
        uint8_t semanticIndex;
    };

    struct GDKGeometryVertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 texCoord;
    };

#pragma pack(pop)

    std::shared_ptr<IGeometry> geo;
    std::vector<PositionNormalTangentTextureVertex> vertices;

    uint8_t* buf = buffer.data();
    GDKGeometryVertex* v = nullptr;
    uint32_t* indices = nullptr;
    GeometryContentHeader* header = (GeometryContentHeader*)buf;
    if (header->Version != GeometryContentVersion)
    {
        throw std::exception();
    }

    buf += sizeof(GeometryContentHeader);
    buf += sizeof(AttributeDesc)* header->AttributeCount;
    v = (GDKGeometryVertex*)buf;

    for (uint32_t i = 0; i < header->VertexCount; i++)
    {
        PositionNormalTangentTextureVertex pntt;
        pntt.Normal = v[i].normal;
        pntt.Position = v[i].position;
        pntt.TexCoord = v[i].texCoord;
        vertices.push_back(pntt);
    }

    buf += header->VertexCount * header->VertexStride;
    indices = (uint32_t*)buf;

    ScaleGeometryToHeight((uint32_t)vertices.size(), VertexFormat::PositionNormalTangentTexture, vertices.data(), targetHeight);
    GenerateTangents((uint32_t)vertices.size(), VertexFormat::PositionNormalTangentTexture, vertices.data(), header->IndexCount, indices);

    geo = graphics->CreateGeometry((uint32_t)vertices.size()/header->FrameCount, VertexFormat::PositionNormalTangentTexture, vertices.data(), header->IndexCount/header->FrameCount, indices);
    if (_verticesLoadedCallback)
    {
        _verticesLoadedCallback(_verticesLoadedCallbackCallerContext, vertices.data(), (uint32_t)vertices.size(), indices, header->IndexCount, transform);
    }
    
    return geo;
}

_Use_decl_annotations_
std::shared_ptr<IGeometry> AssetLoader::LoadGeometryFromOBJFile(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* filename, const Transform& transform)
{
    std::vector<uint8_t> buffer;
    ReadFileToBuffer(filename, buffer);

    std::shared_ptr<IGeometry> geo;
    std::vector<PositionNormalTangentTextureVertex> vertices;
    std::vector<uint32_t> indices;

    std::string textbuffer = (char*)buffer.data();
    std::vector<Vector3> v;
    std::vector<Vector3> n;
    std::vector<Vector2> t;

    std::vector< std::basic_string<char> > x;
    std::split(textbuffer, x, '\n', true);

    for (size_t i = 0; i < x.size(); i++)
    {
        if (x[i].find("v ") == 0)
        {
            Vector3 vertex;
            sscanf_s(x[i].c_str(), "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            v.push_back(vertex);
        }
        else if (x[i].find("vn ") == 0)
        {
            Vector3 normal;
            sscanf_s(x[i].c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
            n.push_back(normal);
        }
        else if (x[i].find("vt ") == 0)
        {
            Vector2 texCoord;
            sscanf_s(x[i].c_str(), "%*s %f %f", &texCoord.x, &texCoord.y);
            t.push_back(texCoord);
        }
        else if (x[i].find("f ") == 0)
        {
            std::vector< std::basic_string<char> > faceVertices;
            std::split(x[i], faceVertices, ' ', true);
            PositionNormalTangentTextureVertex firstFaceVertex;
            for (size_t f = 1 /*skip first*/; f < faceVertices.size(); f++)
            {
                PositionNormalTangentTextureVertex faceVertex;
                int vIndex = 0;
                int tIndex = 0;
                int nIndex = 0;
                std::vector< std::basic_string<char> > vtn;
                std::split(faceVertices[f], vtn, '/', true);
                vIndex = atoi(vtn[0].c_str()) - 1;
                tIndex = atoi(vtn[1].c_str()) - 1;
                nIndex = atoi(vtn[2].c_str()) - 1;
    
                faceVertex.Position.x = v[vIndex].x; faceVertex.Position.y = v[vIndex].y; faceVertex.Position.z = v[vIndex].z;
                faceVertex.TexCoord.x = t[tIndex].x; faceVertex.TexCoord.y = t[tIndex].y;
                faceVertex.Normal.x = n[nIndex].x; faceVertex.Normal.y = n[nIndex].y; faceVertex.Normal.z = n[nIndex].z;
    
                if (f > 3)
                {
                    // triangulate vertex before adding
                    vertices.push_back(firstFaceVertex);
                    vertices.push_back(vertices[vertices.size() - 2]);
                    vertices.push_back(faceVertex);
                }
                else
                {
                    vertices.push_back(faceVertex);
                }
    
                // cache the first vertex for simplified triangulation
                if (f == 1)
                {
                    firstFaceVertex = faceVertex;
                }
            }
        }
    }
    
    // Build indices list
    for (uint32_t i = 0; i < (uint32_t)vertices.size(); i++)
    {
        indices.push_back(i);
    }

    GenerateTangents((uint32_t)vertices.size(), VertexFormat::PositionNormalTangentTexture, vertices.data(), (uint32_t)indices.size(), indices.data());

    geo = graphics->CreateGeometry((uint32_t)vertices.size(), VertexFormat::PositionNormalTangentTexture, vertices.data(), (uint32_t)indices.size(), indices.data());

    if (_verticesLoadedCallback)
    {
        _verticesLoadedCallback(_verticesLoadedCallbackCallerContext, vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), transform);
    }

    return geo;
}

_Use_decl_annotations_
void AssetLoader::ReadFileToBuffer(const wchar_t* path, std::vector<uint8_t>& buffer)
{
    std::ifstream inputfile(path,std::ios::in|std::ios_base::binary|std::ios::ate);
    if (inputfile.is_open())
    {
        buffer.resize((uint32_t)inputfile.tellg());
        inputfile.seekg(0, std::ios::beg);
        inputfile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        inputfile.close();
    }
}

_Use_decl_annotations_
std::shared_ptr<ITexture> AssetLoader::GenerateNormalMapFromTexture(const std::shared_ptr<IGraphicsSystem>& graphics, uint32_t width, uint32_t height, DXGI_FORMAT format, const void* pixels, float amplitude)
{
    ScratchImage image;
    image.Initialize2D(format, static_cast<size_t>(width), static_cast<size_t>(height), 1, 1);
    memcpy_s(image.GetPixels(), image.GetPixelsSize(), pixels, image.GetPixelsSize());

    ScratchImage normalMap;
    ComputeNormalMap(*image.GetImage(0, 0, 0), CNMAP_CHANNEL_LUMINANCE | CNMAP_MIRROR_V, amplitude, DXGI_FORMAT_R8G8B8A8_UNORM, normalMap);

    return graphics->CreateTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, normalMap.GetPixels(), width * sizeof(uint32_t), false);
}

_Use_decl_annotations_
void AssetLoader::SetVerticesLoadedCallback(void* callerContext, VerticesLoaded callback)
{
    _verticesLoadedCallback = callback;
    _verticesLoadedCallbackCallerContext = callerContext;
}
