#ifndef WIN32
#include "GlGeometry.h"
#include <GDKError.h>

#define OFFSET(n) (void *)(0 + n)

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<GlGeometry> GlGeometry::Create(const std::shared_ptr<GeometryContent>& content)
{
    uint32_t stride = content->GetVertexStride();
    uint32_t numVertices = content->GetNumVertices();

    const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);
    const byte_t* normals = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Normal, 0);
    const byte_t* texCoords = content->GetAttributeData(GeometryContent::AttributeType::Float2, GeometryContent::AttributeName::TexCoord, 0);

    CHECK_NOT_NULL(positions);
    CHECK_NOT_NULL(normals);
    CHECK_NOT_NULL(texCoords);

    std::unique_ptr<Vertex[]> vertices(new Vertex[numVertices]);
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        Float3 pos = *reinterpret_cast<const Float3*>(positions);
        Float3 norm = *reinterpret_cast<const Float3*>(normals);
        Float2 tex = *reinterpret_cast<const Float2*>(texCoords);

        positions += stride;
        normals += stride;
        texCoords += stride;

        vertices.get()[i] = Vertex(pos, norm, tex);
    }

    return std::shared_ptr<GlGeometry>(GDKNEW GlGeometry(content->GetNumFrames(), vertices.get(), numVertices, content->GetIndices(), content->GetNumIndices()));
}

_Use_decl_annotations_
std::shared_ptr<GlGeometry> GlGeometry::Create(uint32_t numFrames, const Vertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices)
{
    return std::shared_ptr<GlGeometry>(GDKNEW GlGeometry(numFrames, vertices, numVertices, indices, numIndices));
}

_Use_decl_annotations_
GlGeometry::GlGeometry(uint32_t numFrames, const Vertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices) : 
    _vaoHandle(0),
    _vboHandle(0),
    _iboHandle(0),
    _numVertices(numVertices),
    _numFrames(numFrames),
    _numIndices(numIndices),
    _numVerticesPerFrame(numVertices / numFrames),
    _numIndicesPerFrame(numIndices / numFrames)
{
    // Generate the vertex array object and our two buffers
    CHECK_GL(glGenVertexArrays(1, &_vaoHandle));
    CHECK_TRUE(_vaoHandle != 0);    
    CHECK_GL(glGenBuffers(1, &_vboHandle));
    CHECK_TRUE(_vboHandle != 0);    
    CHECK_GL(glGenBuffers(1, &_iboHandle));
    CHECK_TRUE(_iboHandle != 0);

    // Bind the vertex array so we can configure it
    CHECK_GL(glBindVertexArray(_vaoHandle));

    // Bind both buffers to the array
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, _vboHandle));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboHandle));

    // Set our buffer data
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), static_cast<const void *>(vertices), GL_STATIC_DRAW));
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), static_cast<const void *>(indices), GL_STATIC_DRAW));
        
    // Enable the attribute pointers to read from our vertex buffer.
    // Assuming the shader program is using 0, 1, and 2 for the position, normal, and texCoord attributes respectively.
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position)));
    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal)));
    CHECK_GL(glEnableVertexAttribArray(2));
    CHECK_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord)));

    // Unset the array from the device
    CHECK_GL(glBindVertexArray(0));
}

GlGeometry::~GlGeometry()
{
    if (_vaoHandle)
    {
        CHECK_GL(glDeleteVertexArrays(1, &_vaoHandle));
        _vaoHandle = 0;
    }
    if (_vboHandle)
    {
        CHECK_GL(glDeleteBuffers(1, &_vboHandle));
        _vboHandle = 0;
    }
    if (_iboHandle)
    {
        CHECK_GL(glDeleteBuffers(1, &_iboHandle));
        _iboHandle = 0;
    }
}

void GlGeometry::Bind()
{
    CHECK_GL(glBindVertexArray(_vaoHandle));
}

void GlGeometry::Unbind()
{
    CHECK_GL(glBindVertexArray(0));
}

_Use_decl_annotations_
void GlGeometry::Draw(uint32_t frame)
{
    CHECK_RANGE(frame, 0, _numFrames - 1);

    uint32_t baseIndex = frame * _numIndicesPerFrame;
    void *indexOffsetInBytes = (void *)(baseIndex * sizeof(uint32_t));
    CHECK_GL(glDrawElements(GL_TRIANGLES, _numIndicesPerFrame, GL_UNSIGNED_INT, indexOffsetInBytes));
}
#endif