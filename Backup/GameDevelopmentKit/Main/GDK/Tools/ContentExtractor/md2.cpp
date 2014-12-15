#include "stdafx.h"

struct AnimationFrameInfo
{
    uint32_t startFrame;
    uint32_t endFrame;
    std::string name;
};

void AddAnimationFrame(_In_ std::string name, _In_ size_t frameIndex, _In_ std::vector<AnimationFrameInfo>& animationFrames);

HRESULT SaveMD2AsGeometryContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    std::shared_ptr<GDK::GeometryContent> geometryContent;

    GDK::GeometryContent::Animation* animation = nullptr;
    std::vector<AnimationFrameInfo> animationFrames;

    uint32_t vertexCount = 0;
    uint32_t indiceCount = 0;

    uint32_t numVertices = 0;
    std::unique_ptr<GDK::GeometryContent::Vertex[]> allVertices;

    uint32_t numIndices = 0;
    std::unique_ptr<uint32_t[]> allIndices;

    uint32_t numAnimations = 0;
    std::unique_ptr<GDK::GeometryContent::Animation[]> allAnimations;

    std::ofstream resourceFile(path,std::ios_base::binary);

    ULONG bytesWritten = 0;
    
    MD2_HEADER* header = (MD2_HEADER*)data;
    GDK::GeometryContent::Vertex vertex = {0};
    MD2_VERTEX* vertices = nullptr;
    MD2_TRIANGLE* tris = (MD2_TRIANGLE*)(data + header->ofs_tris);
    MD2_TEXTURE_COORDINATE* textureCoords = (MD2_TEXTURE_COORDINATE*)(data + header->ofs_st);
    MD2_ANIMATION_FRAME* frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames);

    struct md2FrameInfo
    {
        char     FrameName[64];
        uint32_t VertexCount;
        uint32_t IndicesCount;
    };

    struct md2FrameData
    {
        md2FrameInfo frame;
        std::vector<GDK::GeometryContent::Vertex> grvertices;
        std::vector<uint32_t> grindices;
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
    
    // Writing frames
    for (int frameIndex = 0; frameIndex < header->num_frames; frameIndex++)
    {
        frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames + (frameIndex * header->framesize));

        vertices = frames->verts;
        int indice = 0;

        md2FrameData frameData;

        ZeroMemory(frameData.frame.FrameName, sizeof(frameData.frame.FrameName));
        memcpy(frameData.frame.FrameName, frames->name, sizeof(frames->name));

        // traverse triangles
        for (int t = 0; t < header->num_tris; t++)
        {
            // traverse vertices
            for (int v = 0; v < 3; v++)
            {
                GDK::GeometryContent::Vertex vertex = {0};
                vertex.TextureCoord0.y = ((float)textureCoords[tris[t].index_st[v]].t / (float)header->skinheight);
                vertex.TextureCoord0.x = ((float)textureCoords[tris[t].index_st[v]].s / (float)header->skinwidth);

                vertex.Position.x = (vertices[tris[t].index_xyz[v]].v[0] * frames->scale[0]) + frames->translate[0];
                vertex.Position.z = (vertices[tris[t].index_xyz[v]].v[1] * frames->scale[1]) + frames->translate[1];
                vertex.Position.y = (vertices[tris[t].index_xyz[v]].v[2] * frames->scale[2]) + frames->translate[2];

                // calculate normal
                vertex.Normal.x = 0;
                vertex.Normal.y = 0;
                vertex.Normal.z = 0;

                // avoid adding duplicate vertices.  If a duplicate vertex is found, just add an indice
                // for that triangle.
                bool dupeFound = false;
                for (size_t dupeV = 0; dupeV < frameData.grvertices.size(); dupeV++)
                {
                    if (memcmp(&frameData.grvertices[dupeV], &vertex, sizeof(vertex)) == 0)
                    {
                        frameData.grindices.push_back((uint32_t)dupeV);
                        dupeFound = true;
                        break;
                    }
                }

                if (!dupeFound)
                {
                    frameData.grindices.push_back((uint32_t)frameData.grvertices.size());
                    frameData.grvertices.push_back(vertex);
                }
            }
        }

        frameData.frame.IndicesCount = (uint32_t)frameData.grindices.size();
        numIndices += frameData.frame.IndicesCount;

        frameData.frame.VertexCount = (uint32_t)frameData.grvertices.size();
        numVertices += frameData.frame.VertexCount;

        // Generate normals
        CHECKHR(GenerateNormals(frameData.grvertices.data(), frameData.grvertices.size(), frameData.grindices.data(), frameData.grindices.size()), L"Error generating normals for geometry resource");

        md2Frames.push_back(frameData);
    }

    //SaveAsWaveFrontOBJ(L"x64\\debug\\debug_geometry.obj", frameData.grvertices.data(), frameData.grvertices.size(), frameData.grindices.data(), frameData.grindices.size());

    // Collapse unique frame names into proper animation groups
    
    for (size_t f = 0; f < md2Frames.size(); f++)
    {
        const char* frameName = md2Frames[f].frame.FrameName;
        std::string theframeName;
        bool theframeNameComplete = false;

        for(int i = 0; i < lstrlenA(frameName); i++)
        {
            if (isdigit(frameName[i]))
            {
                // Ensure that we capture the first digit of the name
                // because it marks the frame set.
                if (!theframeNameComplete)
                {
                    theframeName += frameName[i];
                    break;
                }
            }
            else
            {
                theframeName += frameName[i];
            }
        }

        AddAnimationFrame(theframeName,f, animationFrames);
    }

    // Collect GDK-specific Animation data
    numAnimations = (uint32_t)animationFrames.size();
    allAnimations.reset(new GDK::GeometryContent::Animation[numAnimations]);
    GDK::GeometryContent::Animation* animations = allAnimations.get();

    for (size_t i = 0; i < animationFrames.size(); i++)
    {
        animations[i].StartFrame = animationFrames[i].startFrame;
        animations[i].EndFrame = animationFrames[i].endFrame;
        lstrcpyA(animations[i].Name, animationFrames[i].name.c_str());
    }

    // Collect all vertices and indices for all frames into single buffers
    allVertices.reset(new GDK::GeometryContent::Vertex[numVertices]);
    allIndices.reset(new uint32_t[numIndices]);

    for (size_t i = 0; i < md2Frames.size(); i++)
    {
        uint32_t allIndicesCount = indiceCount;

        for (size_t v = 0; v < md2Frames[i].grvertices.size(); v++)
        {
            allVertices.get()[vertexCount++] = md2Frames[i].grvertices[v];
        }

        // Indices are 0-based from their vertex in the frame, so these need to be
        // offset accordingly for each additional frames worth of data
        for (size_t n = 0; n < md2Frames[i].grindices.size(); n++)
        {
            allIndices.get()[indiceCount++] = (md2Frames[i].grindices[n] + allIndicesCount);
        }
    }

    geometryContent = GDK::GeometryContent::Create(numVertices, allVertices, numIndices, allIndices, (uint32_t)md2Frames.size(), numAnimations, allAnimations); 
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