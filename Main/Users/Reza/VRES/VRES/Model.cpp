#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

Model::Model(VertexFormat format, ComPtr<ID3D11Buffer>& vertexBuffer, ComPtr<ID3D11Buffer>& indexBuffer,
             uint32_t numVertices, uint32_t numIndices) :
    SceneNode(SceneNodeType::Model), _format(format), _vertexBuffer(vertexBuffer), _indexBuffer(indexBuffer),
    _numVertices(numVertices), _numIndices(numIndices), _material(nullptr)
{
}

Model::~Model()
{
}
