#pragma once

class ResourceFactory : public IContentStudioResourceFactory
{
public:
    ResourceFactory();
    virtual ~ResourceFactory();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioResourceFactory
    HRESULT CreateTextureResource(PCSTR name, ITextureResource** ppResource);
    HRESULT CreateGeometryResource(PCSTR name, IGeometryResource** ppResource);

    static HRESULT Create(IContentStudioResourceFactory** ppFactory);
    static HRESULT CreateCube(PCSTR name, IGeometryResource** ppResource);
    static HRESULT CreatePlane(PCSTR name, IGeometryResource** ppResource);
    static HRESULT CreateW3DWall(PCSTR name, ITextureResource** ppResource);
    static HRESULT CreateW3DE1M1(PCSTR name, IGeometryResource** ppResource);
    static HRESULT CreateTextureResourceFromEmbeddedBitmapResource(PCSTR name, int resourceId, ITextureResource** ppResource);

private:
    long _refcount;
};