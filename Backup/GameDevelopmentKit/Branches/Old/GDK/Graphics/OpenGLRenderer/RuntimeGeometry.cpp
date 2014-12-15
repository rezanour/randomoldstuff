#include "Precomp.h"

using Microsoft::WRL::ComPtr;
using namespace GDK::Content;

namespace GDK {
namespace Graphics {

ComPtr<RuntimeGeometry> GDKAPI RuntimeGeometry::Create(_In_ GDK::Content::IGeometryResource* resource)
{
    CHECK_NOT_NULL(resource, E_INVALIDARG);

    size_t numVertices = 0;
    const GEOMETRY_RESOURCE_VERTEX* vertices = nullptr;
    CHECKHR(resource->GetVertices(reinterpret_cast<uint32_t*>(&numVertices), &vertices));

    std::unique_ptr<Vector3[]> positions(new Vector3[numVertices]);
    std::unique_ptr<Vector3[]> normals(new Vector3[numVertices]);
    std::unique_ptr<Vector2[]> uvs(new Vector2[numVertices]);

    for (size_t i = 0; i < numVertices; ++i)
    {
        positions[i] = *reinterpret_cast<const Vector3*>(&vertices[i].Position);
        normals[i] = *reinterpret_cast<const Vector3*>(&vertices[i].Normal);
        uvs[i] = *reinterpret_cast<const Vector2*>(&vertices[i].TextureCoord0);
    }

    size_t numIndices = 0;
    const uint32_t* indices = nullptr;
    CHECKHR(resource->GetIndices(reinterpret_cast<uint32_t*>(&numIndices), &indices));

    return CreateFromData(numVertices, positions.get(), normals.get(), uvs.get(), numIndices, indices);
}

ComPtr<RuntimeGeometry> GDKAPI RuntimeGeometry::CreateFromData(_In_ size_t numVertices, _In_ const Vector3* positions, _In_ const Vector3* normals, _In_ const Vector2* uvs, _In_ size_t numIndices, _In_ const uint32_t* indices)
{
    CHECK_NOT_NULL(positions, E_INVALIDARG);
    CHECK_NOT_NULL(normals, E_INVALIDARG);
    CHECK_NOT_NULL(uvs, E_INVALIDARG);

    UNREFERENCED_PARAMETER(indices);

    ComPtr<RuntimeGeometry> geometry = Make<RuntimeGeometry>(L"");

    geometry->_numVertices = static_cast<uint32_t>(numVertices);
    geometry->_numIndices = static_cast<uint32_t>(numIndices);

    // Create the overall object
    glGenVertexArrays(1, &geometry->_vertices);

    // Create the attribute buffers
    glGenBuffers(_countof(geometry->_attributes), geometry->_attributes);

    // Start setting up the object
    glBindVertexArray(geometry->_vertices);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, geometry->_attributes[0]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), positions, GL_STATIC_DRAW);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, geometry->_attributes[1]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), normals, GL_STATIC_DRAW);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, geometry->_attributes[2]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), uvs, GL_STATIC_DRAW);

    return geometry;
}

RuntimeGeometry::RuntimeGeometry(_In_ const std::wstring& name) :
    _name(name),
    _isBound(false),
    _vertices(0),
    _numFrames(1),
    _currentFrame(0)
{
    ZeroMemory(_attributes, sizeof(_attributes));
}

RuntimeGeometry::~RuntimeGeometry()
{
    if (_attributes[0] != 0)
    {
        glDeleteBuffers(_countof(_attributes), _attributes);
    }

    if (_vertices != 0)
    {
        glDeleteVertexArrays(1, &_vertices);
    }
}

void RuntimeGeometry::Bind()
{
    _isBound = true;

    glBindVertexArray(_vertices);

    // Position
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _attributes[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _attributes[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture Coords
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _attributes[2]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void RuntimeGeometry::Unbind()
{
    _isBound = false;
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void RuntimeGeometry::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0, _numVertices);
}

} // Graphics
} // GDK