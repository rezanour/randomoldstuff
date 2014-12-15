#include "stdafx.h"

GeometryResource::GeometryResource(PCSTR name) :
    _refcount(1),
    _format(GEOMETRY_RESOURCE_FORMAT_UNKNOWN)
{
    _name = name;
    _id = CoreServices::CreateUniqueId();
}

GeometryResource::~GeometryResource()
{

}

void GeometryResource::Clear()
{
    _format = GEOMETRY_RESOURCE_FORMAT_UNKNOWN;
    _vertices.clear();
    _indices.clear();
}

HRESULT STDMETHODCALLTYPE GeometryResource::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == __uuidof(IGeometryResource))
    {
        *ppvObject = static_cast<IGeometryResource*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE GeometryResource::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE GeometryResource::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

// IGeometryResource
HRESULT GeometryResource::Save(_In_ IStream* pStream)
{
    if (pStream == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ULONG cbWritten = 0;

    GEOMETRY_RESOURCE_FILEHEADER header = {0};
    header.Header.Version = GEOMETRY_RESOURCE_VERSION;
    header.Header.Id    = (__int64)_id;
    StringCchCopyA( header.Header.Name, ARRAYSIZE(header.Header.Name), _name.c_str() );
    header.Format       = (uint32_t)_format;
    header.VertexCount  = (uint32_t)_vertices.size();
    header.IndicesCount = (uint32_t)_indices.size();
    
    hr = pStream->Write(&header, sizeof(header), &cbWritten);
    if (SUCCEEDED(hr))
    {
        hr = pStream->Write(_vertices.data(), (ULONG)_vertices.size() * sizeof(GEOMETRY_RESOURCE_VERTEX), &cbWritten);
    }

    if (SUCCEEDED(hr))
    {
        hr = pStream->Write(_indices.data(), (ULONG)_indices.size() * sizeof(uint32_t), &cbWritten);
    }

    return hr;
}

HRESULT GeometryResource::Load(_In_ IStream* pStream)
{
    if (pStream == nullptr)
    {
        return E_INVALIDARG;
    }

    Clear();

    HRESULT hr = S_OK;
    ULONG cbRead = 0;

    GEOMETRY_RESOURCE_FILEHEADER header = {0};

    hr = pStream->Read(&header, sizeof(header), &cbRead);
    if (SUCCEEDED(hr))
    {
        if (header.Header.Version != GEOMETRY_RESOURCE_VERSION)
        {
            return E_UNEXPECTED;
        }

        _format = header.Format;
        _vertices.resize(header.VertexCount);
        hr = pStream->Read(_vertices.data(), (ULONG)_vertices.size() * sizeof(GEOMETRY_RESOURCE_VERTEX), &cbRead);
    }

    if (SUCCEEDED(hr) && (header.IndicesCount > 0))
    {
        _indices.resize(header.IndicesCount);
        hr = pStream->Read(_indices.data(), (ULONG)_indices.size() * sizeof(uint32_t), &cbRead);
    }

    return hr;
}
    
HRESULT GeometryResource::Initialize(_In_ uint32_t format, _In_ GEOMETRY_RESOURCE_VERTEX* pVertices, _In_ size_t numVertices, _In_opt_ uint32_t* pIndices, _In_opt_ size_t numIndices)
{
    if (pVertices == nullptr || numVertices == 0)
    {
        return E_INVALIDARG;
    }

    Clear();

    HRESULT hr = S_OK;

    _format = format;
    
    _vertices.resize(numVertices);
    memcpy(_vertices.data(), pVertices, _vertices.size() * sizeof(GEOMETRY_RESOURCE_VERTEX));

    // Attempt to copy optional indices buffer
    if (pIndices != nullptr && numIndices > 0)
    {
        _indices.resize(numIndices);
        memcpy(_indices.data(), pIndices, _indices.size() * sizeof(uint32_t));
    }

    return hr;
}

HRESULT GeometryResource::GetTotalVertices(_In_ size_t& totalVertices)
{
    totalVertices = _vertices.size();
    return S_OK;
}

HRESULT GeometryResource::GetTotalIndices(_In_ size_t& totalIndices)
{
    totalIndices = _indices.size();
    return S_OK;
}

HRESULT GeometryResource::GetFormat(_In_ uint32_t& format)
{
    format = _format;
    return S_OK;
}

HRESULT GeometryResource::SetFormat(_In_ uint32_t format)
{
    _format = format;
    return S_OK;
}

HRESULT GeometryResource::SetColor(_In_ DirectX::XMFLOAT4 color)
{
    for (size_t i = 0; i <  _vertices.size(); i++)
    {
        _vertices[i].Color = color;
    }
    return S_OK;
}

HRESULT GeometryResource::GenerateNormals()
{
    _format |= GEOMETRY_RESOURCE_FORMAT_NORMAL;

    // Copied code from Lucid Graphics system
    void*  pFirstPosition = &_vertices[0].Position;
    void*  pFirstNormal   = &_vertices[0].Normal;
    size_t vertexStride   = sizeof(GEOMETRY_RESOURCE_VERTEX); 
    size_t numVertices    = _vertices.size(); 
    void*  pFirstIndex    = &_indices[0];
    size_t indexStride    = sizeof(uint32_t);
    size_t numIndices     = _indices.size();
    bool   flipCross      = false;

    {
        bool fullSizeIndices = (indexStride >= 4);

        byte* pIndices = (byte*)pFirstIndex;
        byte* pPositions = (byte*)pFirstPosition;
        byte* pNormals = (byte*)pFirstNormal;

        uint32 i0, i1, i2;
        DirectX::XMVECTOR v0, v1, v2, normal;

        // zero pass
        for (size_t i = 0; i < numVertices; i++)
        {
            *(DirectX::XMFLOAT3*)(pNormals + (i * vertexStride)) = DirectX::XMFLOAT3(0, 0, 0);
        }

        for (size_t i = 0; i < numIndices; i+=3)
        {
            if (fullSizeIndices)
            {
                i0 = *(uint32*)(pIndices + (i * indexStride));
                i1 = *(uint32*)(pIndices + ((i + 1) * indexStride));
                i2 = *(uint32*)(pIndices + ((i + 2) * indexStride));
            }
            else
            {
                i0 = (uint32)(*(ushort*)(pIndices + (i * indexStride)));
                i1 = (uint32)(*(ushort*)(pIndices + ((i + 1) * indexStride)));
                i2 = (uint32)(*(ushort*)(pIndices + ((i + 2) * indexStride)));
            }

            v0 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i0 * vertexStride)));
            v1 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i1 * vertexStride)));
            v2 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i2 * vertexStride)));

            DirectX::XMFLOAT3* n0 = (DirectX::XMFLOAT3*)(pNormals + (i0 * vertexStride));
            DirectX::XMFLOAT3* n1 = (DirectX::XMFLOAT3*)(pNormals + (i1 * vertexStride));
            DirectX::XMFLOAT3* n2 = (DirectX::XMFLOAT3*)(pNormals + (i2 * vertexStride));

            normal = flipCross ? 
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v0), DirectX::XMVectorSubtract(v1, v0))) :
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v0), DirectX::XMVectorSubtract(v2, v0)));

            DirectX::XMStoreFloat3(n0, DirectX::XMVectorAdd(XMLoadFloat3(n0), normal));
            DirectX::XMStoreFloat3(n1, DirectX::XMVectorAdd(XMLoadFloat3(n1), normal));
            DirectX::XMStoreFloat3(n2, DirectX::XMVectorAdd(XMLoadFloat3(n2), normal));
        }

        // normalize pass
        for (size_t i = 0; i < numVertices; i++)
        {
            DirectX::XMFLOAT3* n = (DirectX::XMFLOAT3*)(pNormals + (i * vertexStride));
            DirectX::XMStoreFloat3(n, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(n)));
        }
    }

    return S_OK;
}

HRESULT GeometryResource::GenerateTangents()
{
    // _format |= GEOMETRY_RESOURCE_FORMAT_TANGENT;
    return E_NOTIMPL;
}

HRESULT GeometryResource::GenerateBitangents()
{
    // _format |= GEOMETRY_RESOURCE_FORMAT_BITANGENT;
    return E_NOTIMPL;
}

HRESULT GeometryResource::GetVertices(GEOMETRY_RESOURCE_VERTEX** ppVertices, size_t& numVertices, uint32_t** ppIndices, size_t& numIndices)
{
    if (ppVertices == nullptr)
    {
        return E_INVALIDARG;
    }

    numVertices = _vertices.size();
    numIndices = _indices.size();

    *ppVertices = _vertices.data();

    if(ppIndices)
    {
        *ppIndices = _indices.data();
    }

    return S_OK;
}

HRESULT GeometryResource::GetId(unsigned __int64& id)
{
    id = _id;
    return S_OK;
}

HRESULT GeometryResource::Create(PCSTR name, IGeometryResource** ppResource)
{
    if (ppResource == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    *ppResource = new GeometryResource(name);
    if (!*ppResource)
    {
        return E_OUTOFMEMORY;
    }

    return hr;
}
