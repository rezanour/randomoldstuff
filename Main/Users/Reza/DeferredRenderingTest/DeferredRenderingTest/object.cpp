#include "precomp.h"
#include "object.h"
#include "vertex.h"

Object::Object() :
    _position(0, 0, 0),
    _orientation(0, 0, 0, 1),
    _scale(1, 1, 1),
    _worldDirty(true),
    _indexCount(0),
    _vertexFormat(VertexFormat::PositionNormal)
{
}

std::shared_ptr<Object> Object::CreateCube(ID3D11Device* device, float width, float height, float depth)
{
    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    VertexPositionNormalTexture vertices[] = 
    {
        { XMFLOAT3(-w, -h, -d), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 3) },
        { XMFLOAT3(-w,  h, -d), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
        { XMFLOAT3( w,  h, -d), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
        { XMFLOAT3( w, -h, -d), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 3) },

        { XMFLOAT3( w, -h,  d), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 3) },
        { XMFLOAT3( w,  h,  d), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 0) },
        { XMFLOAT3(-w,  h,  d), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 0) },
        { XMFLOAT3(-w, -h,  d), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 3) },

        { XMFLOAT3(-w, -h,  d), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 3) },
        { XMFLOAT3(-w,  h,  d), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3(-w,  h, -d), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3(-w, -h, -d), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 3) },

        { XMFLOAT3(w, -h, -d), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 3) },
        { XMFLOAT3(w,  h, -d), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3(w,  h,  d), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3(w, -h,  d), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 3) },

        { XMFLOAT3(-w, h, -d), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1) },
        { XMFLOAT3(-w, h,  d), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3( w, h,  d), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3( w, h, -d), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1) },

        { XMFLOAT3(-w, -h, d),  XMFLOAT3(0, -1, 0), XMFLOAT2(0, 1) },
        { XMFLOAT3(-w, -h, -d), XMFLOAT3(0, -1, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3( w, -h, -d), XMFLOAT3(0, -1, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3( w, -h, d),  XMFLOAT3(0, -1, 0), XMFLOAT2(1, 1) },
    };

    uint32_t indices[] = 
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };

    std::shared_ptr<Object> obj(new Object());

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(VertexPositionNormalTexture) * _countof(vertices);
    bd.StructureByteStride = sizeof(VertexPositionNormalTexture);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = bd.ByteWidth;

    device->CreateBuffer(&bd, &init, &obj->_vertexBuffer);

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(uint32_t) * _countof(indices);
    bd.StructureByteStride = sizeof(uint32_t);

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = bd.ByteWidth;

    device->CreateBuffer(&bd, &init, &obj->_indexBuffer);

    obj->_indexCount = _countof(indices);
    obj->_vertexFormat = VertexFormat::PositionNormalTexture;

    return obj;
}

std::shared_ptr<Object> Object::CreateQuad(ID3D11Device* device, float x, float y, float width, float height)
{
    // normalize inputs to [-1, 1] post projection space
    float left = x * 2 - 1;
    float top = y * -2 + 1;
    float right = (x + width) * 2 - 1;
    float bottom = (y + height) * -2 + 1;

    VertexPositionNormal vertices[] = 
    {
        { XMFLOAT3(left, bottom, 0), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3(left, top, 0), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3(right, top, 0), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3(right, bottom, 0), XMFLOAT3(0, 0, 1) },
    };

    uint32_t indices[] = 
    {
        0, 1, 2, 0, 2, 3,
    };

    std::shared_ptr<Object> obj(new Object());

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(VertexPositionNormal) * _countof(vertices);
    bd.StructureByteStride = sizeof(VertexPositionNormal);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = bd.ByteWidth;

    device->CreateBuffer(&bd, &init, &obj->_vertexBuffer);

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(uint32_t) * _countof(indices);
    bd.StructureByteStride = sizeof(uint32_t);

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = bd.ByteWidth;

    device->CreateBuffer(&bd, &init, &obj->_indexBuffer);

    obj->_indexCount = _countof(indices);
    obj->_vertexFormat = VertexFormat::PositionNormal;

    return obj;
}

const XMFLOAT3& Object::GetPosition() const
{
    return _position;
}

const XMFLOAT4& Object::GetOrientation() const
{
    return _orientation;
}

const XMFLOAT3& Object::GetScale() const
{
    return _scale;
}

const XMFLOAT4X4& Object::GetWorld() const
{
    if (_worldDirty)
    {
        XMStoreFloat4x4(&_world,
            XMMatrixMultiply(
                XMMatrixMultiply(
                    XMMatrixScalingFromVector(XMLoadFloat3(&_scale)),
                    XMMatrixRotationQuaternion(XMLoadFloat4(&_orientation))),
                    XMMatrixTranslationFromVector(XMLoadFloat3(&_position))));

        _worldDirty = false;
    }

    return _world;
}

void Object::SetPosition(const XMFLOAT3& v)
{
    _position = v;
    _worldDirty = true;
}

void Object::SetPosition(CXMVECTOR v)
{
    XMStoreFloat3(&_position, v);
    _worldDirty = true;
}

void Object::SetOrientation(const XMFLOAT4& v)
{
    _orientation = v;
    _worldDirty = true;
}

void Object::SetOrientation(CXMVECTOR v)
{
    XMStoreFloat4(&_orientation, v);
    _worldDirty = true;
}

void Object::SetScale(const XMFLOAT3& v)
{
    _scale = v;
    _worldDirty = true;
}

void Object::SetScale(CXMVECTOR v)
{
    XMStoreFloat3(&_scale, v);
    _worldDirty = true;
}

const ComPtr<ID3D11Buffer>& Object::GetVertexBuffer() const
{
    return _vertexBuffer;
}

const ComPtr<ID3D11Buffer>& Object::GetIndexBuffer() const
{
    return _indexBuffer;
}

uint32_t Object::GetIndexCount() const
{
    return _indexCount;
}

VertexFormat Object::GetVertexFormat() const
{
    return _vertexFormat;
}
