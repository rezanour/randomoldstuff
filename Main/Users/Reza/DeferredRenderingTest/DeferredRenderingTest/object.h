#pragma once

class Texture2D;
enum class VertexFormat;

class Object
{
public:
    // Creation
    Object();

    static std::shared_ptr<Object> CreateCube(ID3D11Device* device, float width, float height, float depth);
    static std::shared_ptr<Object> CreateQuad(ID3D11Device* device, float x, float y, float width, float height); 

    // Transform
    const XMFLOAT3& GetPosition() const;
    const XMFLOAT4& GetOrientation() const;
    const XMFLOAT3& GetScale() const;
    const XMFLOAT4X4& GetWorld() const;

    void SetPosition(const XMFLOAT3& v);
    void SetPosition(CXMVECTOR v);

    void SetOrientation(const XMFLOAT4& v);
    void SetOrientation(CXMVECTOR v);

    void SetScale(const XMFLOAT3& v);
    void SetScale(CXMVECTOR v);

    // Geometry
    const ComPtr<ID3D11Buffer>& GetVertexBuffer() const;
    const ComPtr<ID3D11Buffer>& GetIndexBuffer() const;
    uint32_t GetIndexCount() const;
    VertexFormat GetVertexFormat() const;



private:
    // Transform
    XMFLOAT3 _position;
    XMFLOAT4 _orientation; // quaternion
    XMFLOAT3 _scale;
    mutable XMFLOAT4X4 _world;
    mutable bool _worldDirty;

    // Geometry
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Buffer> _indexBuffer;
    uint32_t _indexCount;
    VertexFormat _vertexFormat;
};
