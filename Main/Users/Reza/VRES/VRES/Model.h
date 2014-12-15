#pragma once

#include "Scene.h"

namespace VRES
{
    class Material;

    enum class TextureType
    {
        Diffuse = 0,
        Normal,
        Max
    };

    class Model : NonCopyable<Model>, public SceneNode
    {
    public:
        Model(VertexFormat format, ComPtr<ID3D11Buffer>& vertexBuffer, ComPtr<ID3D11Buffer>& indexBuffer,
              uint32_t numVertices, uint32_t numIndices);
        ~Model();

        const std::shared_ptr<Material>& Material() const { return _material; }
        void SetMaterial(const std::shared_ptr<VRES::Material> material) { _material = material; }

        const ComPtr<ID3D11ShaderResourceView>& Texture(TextureType type) const { return _textures[(uint32_t)type]; }
        void SetTexture(TextureType type, const ComPtr<ID3D11ShaderResourceView>& srv) { _textures[(uint32_t)type] = srv; }

        VertexFormat Format() const { return _format; }
        const ComPtr<ID3D11Buffer>& VertexBuffer() const { return _vertexBuffer; }
        const ComPtr<ID3D11Buffer>& IndexBuffer() const { return _indexBuffer; }
        uint32_t NumVertices() const { return _numVertices; }
        uint32_t NumIndices() const { return _numIndices; }

    private:
        std::shared_ptr<VRES::Material> _material;
        ComPtr<ID3D11ShaderResourceView> _textures[(uint32_t)TextureType::Max];
        VertexFormat _format;
        ComPtr<ID3D11Buffer> _vertexBuffer;
        ComPtr<ID3D11Buffer> _indexBuffer;
        uint32_t _numVertices;
        uint32_t _numIndices;
    };
}
