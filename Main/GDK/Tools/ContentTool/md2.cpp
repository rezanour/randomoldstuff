#include "md2.h"
#include "helpers.h"

#pragma pack(push, 1)

struct geometryContentVertex_t
{
    GDK::Float3 position;
    GDK::Float3 normal;
    GDK::Float2 texCoord;
};

struct animationFrame_t
{
    uint32_t startFrame;
    uint32_t endFrame;
    std::string name;
};

struct md2FrameData_t
{
    char     FrameName[64];
    uint32_t indexListStart;    // first index in the allIndices list where this frame starts
    uint32_t indexListEnd;      // last index in the allIndices list where this frame ends
};

#pragma pack(pop)

void AddAnimationFrame(_In_ std::string name, _In_ size_t frameIndex, _In_ std::vector<animationFrame_t>& animationFrames)
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
        animationFrame_t newFrame;
        newFrame.name = name;
        newFrame.startFrame = (uint32_t)frameIndex;
        newFrame.endFrame = (uint32_t)frameIndex;
        animationFrames.push_back(newFrame);
    }
}

void ConvertMD2ToGeometryContent(_In_ std::vector<uint8_t>& md2, _Inout_ std::shared_ptr<GDK::GeometryContent>& content)
{
    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);
    std::unique_ptr<GDK::GeometryContent::Animation[]> animations;
    std::unique_ptr<byte_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;

    std::vector<geometryContentVertex_t> allVertices;
    std::vector<uint32_t> allIndices;
    uint32_t validateIndicesPerFrame = 0;
    uint32_t animationCount = 0;
    std::vector<animationFrame_t> animationFrames;

    md2_fileheader_t* fileHeader = (md2_fileheader_t*)md2.data();
    md2_vertex_t* vertices = nullptr;
    md2_triangle_t* tris = (md2_triangle_t*)(md2.data() + fileHeader->ofs_tris);
    md2_texture_coordinate_t* textureCoords = (md2_texture_coordinate_t*)(md2.data() + fileHeader->ofs_st);
    md2_animation_frame_t* frames = (md2_animation_frame_t*)(md2.data() + fileHeader->ofs_frames);

    std::vector<md2FrameData_t> md2Frames;

    // MD2's are oriented so that the forward vector points along positive x.
    // GDK is designed so that forward vectors point along positive z.
    // This means we need to rotate all the vertices on conversion by 90 degrees
    GDK::Matrix rotate(GDK::Matrix::CreateRotationY(-GDK::Math::ToRadians(90)));
    GDK::Vector3 pos;

    for (int frameIndex = 0; frameIndex < fileHeader->num_frames; frameIndex++)
    {
        frames = (md2_animation_frame_t*)(md2.data() + fileHeader->ofs_frames + (frameIndex * fileHeader->framesize));
        vertices = frames->verts;
        md2FrameData_t frameData;
        memset(frameData.FrameName, 0, sizeof(frameData.FrameName));
        memcpy(frameData.FrameName, frames->name, sizeof(frames->name));

        frameData.indexListStart = (uint32_t)allIndices.size();
        size_t startVertex = allVertices.size();

        // traverse triangles
        for (int t = 0; t < fileHeader->num_tris; t++)
        {
            // traverse vertices
            for (int v = 0; v < 3; v++)
            {
                geometryContentVertex_t vertex;

                // store texture coordinates
                vertex.texCoord.y = ((float)textureCoords[tris[t].index_st[v]].t / (float)fileHeader->skinheight);
                vertex.texCoord.x = ((float)textureCoords[tris[t].index_st[v]].s / (float)fileHeader->skinwidth);

                // store position
                pos.x = (vertices[tris[t].index_xyz[v]].v[0] * frames->scale[0]) + frames->translate[0];
                pos.z = (vertices[tris[t].index_xyz[v]].v[1] * frames->scale[1]) + frames->translate[1];
                pos.y = (vertices[tris[t].index_xyz[v]].v[2] * frames->scale[2]) + frames->translate[2];

                pos = GDK::Matrix::Transform(pos, rotate);

                vertex.position.x = pos.x;
                vertex.position.y = pos.y;
                vertex.position.z = pos.z;

                // zero normal, since it's computed later
                vertex.normal.x = 0;
                vertex.normal.y = 0;
                vertex.normal.z = 0;

                bool exists = false;
                if (allVertices.size() > startVertex)
                {
                    const geometryContentVertex_t* existing = &allVertices[startVertex];
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

        md2Frames.push_back(frameData);
    }

    // Done processing vertices and indices, time to generate normals for all the vertices
    GenerateNormals(reinterpret_cast<const byte_t*>(&allVertices[0].position), sizeof(geometryContentVertex_t), allVertices.size(), allIndices.data(), allIndices.size(), reinterpret_cast<byte_t*>(&allVertices[0].normal));

    // Collapse unique frame names into proper animation groups
    for (size_t f = 0; f < md2Frames.size(); f++)
    {
        const char* frameName = md2Frames[f].FrameName;
        std::string theframeName;
        for(int i = 0; i < strlen(frameName); i++)
        {
            if (isdigit(frameName[i]))
            {
                if ((i+1) != strlen(frameName))
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
        strcpy_s(animations[i].Name, animationFrames[i].name.c_str());
    }

    // Convert the vertices and indices into buffers appropriate for GDK
    vertexData.reset(new byte_t[sizeof(geometryContentVertex_t) * allVertices.size()]);
    indexData.reset(new uint32_t[allIndices.size()]);

    memcpy(vertexData.get(), allVertices.data(), sizeof(geometryContentVertex_t) * allVertices.size());
    memcpy(indexData.get(), allIndices.data(), sizeof(uint32_t) * allIndices.size());

    content = GDK::GeometryContent::Create(attributeCount, attributes, sizeof(geometryContentVertex_t), (uint32_t)allVertices.size(), vertexData, (uint32_t)allIndices.size(), indexData, (uint32_t)md2Frames.size(), animationCount, animations); 
}