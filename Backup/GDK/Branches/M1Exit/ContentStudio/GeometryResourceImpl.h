#pragma once

class GeometryResource : public IGeometryResource
{
public:
    GeometryResource(PCSTR name);
    virtual ~GeometryResource();
    
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IGeometryResource
    HRESULT Save(_In_ IStream* pStream);
    HRESULT Load(_In_ IStream* pStream);
    
    HRESULT Initialize(_In_ uint32_t format, _In_ GEOMETRY_RESOURCE_VERTEX* pVertices, _In_ size_t numVertices, _In_opt_ uint32_t* pIndices, _In_opt_ size_t numIndices);
    
    HRESULT GetTotalVertices(_In_ size_t& totalVertices);
    HRESULT GetTotalIndices(_In_ size_t& totalIndices);
    HRESULT GetFormat(_In_ uint32_t& format);

    HRESULT SetFormat(_In_ uint32_t format);
    HRESULT SetColor(_In_ L3DMath::Vector4 color);

    HRESULT GenerateNormals();
    HRESULT GenerateTangents();
    HRESULT GenerateBitangents();

    HRESULT GetVertices(GEOMETRY_RESOURCE_VERTEX** ppVertices, size_t& numVertices, uint32_t** ppIndices, size_t& numIndices);

    HRESULT GetId(unsigned __int64& id);

    static HRESULT Create(PCSTR name, IGeometryResource** ppResource);

private:
    void Clear();

private:
    long _refcount;
    uint32_t _format;
    std::string _name;
    int64 _id;
    std::vector<GEOMETRY_RESOURCE_VERTEX> _vertices;
    std::vector<uint32_t> _indices;
};