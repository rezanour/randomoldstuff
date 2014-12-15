#include "stdafx.h"
using namespace Microsoft::WRL;

struct AnimationFrameInfo
{
    uint32_t startFrame;
    uint32_t endFrame;
    std::string name;
};

void AddAnimationFrame(_In_ std::string name, _In_ size_t frameIndex, _In_ std::vector<AnimationFrameInfo>& animationFrames);

HRESULT SaveMD2FileAsGeometryContent(const wchar_t* path, const wchar_t* filePath)
{
    if (!path || !filePath)
    {
        return E_INVALIDARG;
    }
    std::vector<byte_t> md2Data;
    HRESULT hr = S_OK;
    ComPtr<IStream> md2File;
    ULONG bytesRead = 0;

    CHECKHR(SHCreateStreamOnFile(filePath, STGM_READ, &md2File), L"Error opening MD2 file");
    md2Data.resize(GetStreamSize(md2File.Get()));
    CHECKHR(md2File->Read(md2Data.data(), (ULONG)md2Data.size(), &bytesRead), L"Error reading entire MD2 file");
    CHECKHR(SaveMD2AsGeometryContent(path, " ", md2Data.data(), (uint32_t)md2Data.size()), L"Error saving Geometry content from MD2 file");
Exit:

    return hr;
}

HRESULT SaveMD2AsGeometryContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    std::shared_ptr<GDK::GeometryContent> geometryContent;

    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

#pragma pack(push, 1)
    struct GeometryContentMD2Vertex
    {
        GDK::Float3 position;
        GDK::Float3 normal;
        GDK::Float2 texCoord;
    };
#pragma pack(pop)

    std::vector<GeometryContentMD2Vertex> allVertices;
    std::vector<uint32_t> allIndices;

    std::unique_ptr<byte_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;

    uint32_t validateIndicesPerFrame = 0;
    uint32_t animationCount = 0;
    std::vector<AnimationFrameInfo> animationFrames;
    std::unique_ptr<GDK::GeometryContent::Animation[]> animations;

    std::ofstream resourceFile(path,std::ios_base::binary);

    ULONG bytesWritten = 0;
    
    MD2_HEADER* header = (MD2_HEADER*)data;
    MD2_VERTEX* vertices = nullptr;
    MD2_TRIANGLE* tris = (MD2_TRIANGLE*)(data + header->ofs_tris);
    MD2_TEXTURE_COORDINATE* textureCoords = (MD2_TEXTURE_COORDINATE*)(data + header->ofs_st);
    MD2_ANIMATION_FRAME* frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames);

    struct md2FrameData
    {
        char     FrameName[64];
        uint32_t indexListStart;    // first index in the allIndices list where this frame starts
        uint32_t indexListEnd;      // last index in the allIndices list where this frame ends
    };

    std::vector<md2FrameData> md2Frames;

    printf("MD2 details\n"
            "  name:       %s\n"
            "  skinwidth:  %d\n"
            "  skinheight: %d\n"
            "  framesize:  %d\n"
            "  num_skins:  %d\n"
            "  num_xyz:    %d\n"
            "  num_st:     %d\n"
            "  num_tris:   %d\n"
            "  num_glcmds: %d\n"
            "  num_frames: %d\n", name, header->skinwidth, header->skinheight, header->framesize, header->num_skins, 
                                  header->num_xyz, header->num_st, header->num_tris, header->num_glcmds, header->num_frames);

    // MD2 to geometry resource algorithm:
    //
    // MD2s are stored as a sequence of triangles per frame. The number of triangles must be the same for every frame.
    // Our goal is to collapse all of the vertices into a single vertex list, and store only a unique index list per frame.
    // We're going to generate our own normals for the vertices, so we can collapse based only on position & texture coordinates.

    for (int frameIndex = 0; frameIndex < header->num_frames; frameIndex++)
    {
        frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames + (frameIndex * header->framesize));

        vertices = frames->verts;

        md2FrameData frameData;

        ZeroMemory(frameData.FrameName, sizeof(frameData.FrameName));
        memcpy(frameData.FrameName, frames->name, sizeof(frames->name));

        frameData.indexListStart = (uint32_t)allIndices.size();

        // We don't gain much by folding vertices across the entire data set. The majority of the 
        // within is from doing it within a frame, so let's store off the start of the frame and do it from
        // here
        size_t startVertex = allVertices.size();

        // traverse triangles
        for (int t = 0; t < header->num_tris; t++)
        {
            // traverse vertices
            for (int v = 0; v < 3; v++)
            {
                GeometryContentMD2Vertex vertex;

                // store texture coordinates
                vertex.texCoord.y = ((float)textureCoords[tris[t].index_st[v]].t / (float)header->skinheight);
                vertex.texCoord.x = ((float)textureCoords[tris[t].index_st[v]].s / (float)header->skinwidth);

                // store position
                vertex.position.x = (vertices[tris[t].index_xyz[v]].v[0] * frames->scale[0]) + frames->translate[0];
                vertex.position.z = (vertices[tris[t].index_xyz[v]].v[1] * frames->scale[1]) + frames->translate[1];
                vertex.position.y = (vertices[tris[t].index_xyz[v]].v[2] * frames->scale[2]) + frames->translate[2];

                // zero normal, since it's computed later
                vertex.normal.x = 0;
                vertex.normal.y = 0;
                vertex.normal.z = 0;

                bool exists = false;
                if (allVertices.size() > startVertex)
                {
                    const GeometryContentMD2Vertex* existing = &allVertices[startVertex];
                    for (size_t i = startVertex; i < allVertices.size(); ++i, ++existing)
                    {
                        if (memcmp(existing, &vertex, sizeof(vertex)) == 0)
                        {
                            allIndices.push_back((uint32_t)i);
                            exists = true;
                            break;
                        }
                    }
                }

                if (!exists)
                {
                    allIndices.push_back((uint32_t)allVertices.size());
                    allVertices.push_back(vertex);
                }
            }
        }

        frameData.indexListEnd = (uint32_t)allIndices.size();

        // Validate that all frames have the same number of indices
        if (validateIndicesPerFrame == 0)
        {
            validateIndicesPerFrame = frameData.indexListEnd - frameData.indexListStart;
        }
        else
        {
            if (validateIndicesPerFrame != frameData.indexListEnd - frameData.indexListStart)
            {
                CHECKHR(E_FAIL, L"Error! frames have nonuniform index counts!");
            }
        }

        md2Frames.push_back(frameData);
    }

    // Done processing vertices and indices, time to generate normals for all the vertices
    CHECKHR(GenerateNormals(reinterpret_cast<const byte_t*>(&allVertices[0].position), sizeof(GeometryContentMD2Vertex), allVertices.size(), allIndices.data(), allIndices.size(), reinterpret_cast<byte_t*>(&allVertices[0].normal)), L"Error generating normals for geometry resource");

    //SaveAsWaveFrontOBJ(L"x64\\debug\\debug_geometry.obj", frameData.grvertices.data(), frameData.grvertices.size(), frameData.grindices.data(), frameData.grindices.size());

    // Collapse unique frame names into proper animation groups
    for (size_t f = 0; f < md2Frames.size(); f++)
    {
        const char* frameName = md2Frames[f].FrameName;
        std::string theframeName;
        bool theframeNameComplete = false;

        for(int i = 0; i < lstrlenA(frameName); i++)
        {
            if (isdigit(frameName[i]))
            {
                // Ensure that we capture the first digit of the name
                // because it marks the frame set.
                if ((i+1) != lstrlenA(frameName))
                {
                    theframeName += frameName[i];
                }
                break;
            }
            else
            {
                theframeName += frameName[i];
            }
        }

        AddAnimationFrame(theframeName,f, animationFrames);
    }

    // Collect GDK-specific Animation data
    animationCount = (uint32_t)animationFrames.size();
    animations.reset(new GDK::GeometryContent::Animation[animationCount]);

    for (size_t i = 0; i < animationFrames.size(); i++)
    {
        animations[i].StartFrame = animationFrames[i].startFrame;
        animations[i].EndFrame = animationFrames[i].endFrame;
        lstrcpyA(animations[i].Name, animationFrames[i].name.c_str());
    }

    // Convert the vertices and indices into buffers appropriate for GDK
    vertexData.reset(new byte_t[sizeof(GeometryContentMD2Vertex) * allVertices.size()]);
    indexData.reset(new uint32_t[allIndices.size()]);

    memcpy(vertexData.get(), allVertices.data(), sizeof(GeometryContentMD2Vertex) * allVertices.size());
    memcpy(indexData.get(), allIndices.data(), sizeof(uint32_t) * allIndices.size());

    geometryContent = GDK::GeometryContent::Create(attributeCount, attributes, sizeof(GeometryContentMD2Vertex), (uint32_t)allVertices.size(), vertexData, (uint32_t)allIndices.size(), indexData, (uint32_t)md2Frames.size(), animationCount, animations); 
    geometryContent->Save(resourceFile);

Exit:

    return hr;
}

void AddAnimationFrame(_In_ std::string name, _In_ size_t frameIndex, _In_ std::vector<AnimationFrameInfo>& animationFrames)
{
    bool foundFrame = false;
    for (size_t i = 0; i < animationFrames.size(); i++)
    {
        if (animationFrames[i].name == name)
        {
            animationFrames[i].endFrame = (uint32_t)frameIndex;
            foundFrame = true;
            break;
        }
    }

    if (!foundFrame)
    {
        AnimationFrameInfo newFrame;
        newFrame.name = name;
        newFrame.startFrame = (uint32_t)frameIndex;
        newFrame.endFrame = (uint32_t)frameIndex;
        animationFrames.push_back(newFrame);
    }
}