#include "stdafx.h"

#pragma warning(disable : 4482) // disables warning 'nonstandard extension used: enum...etc'

ResourceFactory::ResourceFactory() :
    _refcount(1)
{

}

ResourceFactory::~ResourceFactory()
{

}

HRESULT STDMETHODCALLTYPE ResourceFactory::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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
    else if (iid == __uuidof(IContentStudioResourceFactory))
    {
        *ppvObject = static_cast<IContentStudioResourceFactory*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ResourceFactory::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ResourceFactory::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ResourceFactory::CreateTextureResource(PCSTR name, ITextureResource** ppResource)
{
    return DirectXTextureResource::Create(name, ppResource);
}

HRESULT ResourceFactory::CreateGeometryResource(PCSTR name, IGeometryResource** ppResource)
{
    return GeometryResource::Create(name, ppResource);
}

HRESULT ResourceFactory::Create(IContentStudioResourceFactory** ppFactory)
{
    if (ppFactory == nullptr)
    {
        return E_INVALIDARG;
    }

    ResourceFactory* pFactory = new ResourceFactory();
    if (pFactory == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    *ppFactory = pFactory;

    return S_OK;
}

void AddCubeVertices(int x, int y, std::vector<GEOMETRY_RESOURCE_VERTEX>& cubeVertices, std::vector<uint32_t>& cubeIndices)
{
    struct CubeVertex
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMFLOAT2 t;
    };

    // Create geometry for a simple cube
    CubeVertex vertices[] = {
            {DirectX::XMFLOAT3(-0.5, 0.5, -0.5),  DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(0.5, 0.5, -0.5),   DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(0.5, -0.5, -0.5),  DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT2(0, 1)},
            {DirectX::XMFLOAT3(0.5, 0.5, 0.5),    DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(-0.5, 0.5, 0.5),   DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(-0.5, -0.5, 0.5),  DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(0.5, -0.5, 0.5),   DirectX::XMFLOAT2(0, 1)},
            {DirectX::XMFLOAT3(-0.5, 0.5, 0.5),   DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(0.5, 0.5, 0.5),    DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(0.5, 0.5, -0.5),   DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(-0.5, 0.5, -0.5),  DirectX::XMFLOAT2(0, 1)},
            {DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(0.5, -0.5, -0.5),  DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(0.5, -0.5, 0.5),   DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(-0.5, -0.5, 0.5),  DirectX::XMFLOAT2(0, 1)},
            {DirectX::XMFLOAT3(-0.5, 0.5, 0.5),   DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(-0.5, 0.5, -0.5),  DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(-0.5, -0.5, -0.5), DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(-0.5, -0.5, 0.5),  DirectX::XMFLOAT2(0, 1)},
            {DirectX::XMFLOAT3(0.5, 0.5, -0.5),   DirectX::XMFLOAT2(0, 0)},
            {DirectX::XMFLOAT3(0.5, 0.5, 0.5),    DirectX::XMFLOAT2(1, 0)},
            {DirectX::XMFLOAT3(0.5, -0.5, 0.5),   DirectX::XMFLOAT2(1, 1)},
            {DirectX::XMFLOAT3(0.5, -0.5, -0.5),  DirectX::XMFLOAT2(0, 1)},
        };

    uint32 indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < ARRAYSIZE(vertices); i++)
    {
        GEOMETRY_RESOURCE_VERTEX grv;
        grv.Position.x      = vertices[i].v.x + x;
        grv.Position.y      = vertices[i].v.y;
        grv.Position.z      = vertices[i].v.z + y;
        grv.TextureCoord0.x = vertices[i].t.x;
        grv.TextureCoord0.y = vertices[i].t.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < ARRAYSIZE(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

HRESULT ResourceFactory::CreateW3DE1M1(PCSTR name, IGeometryResource** ppResource)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IContentStudioResourceFactory> spFactory;

    // Create a resource factory to use to create the map geometry
    hr = ResourceFactory::Create(&spFactory);
    if (SUCCEEDED(hr))
    {
        std::vector<GEOMETRY_RESOURCE_VERTEX> cubeVertices;
        std::vector<uint32_t> cubeIndices;
        stde::com_ptr<IGeometryResource> spResource;

        // import geometry resource
        HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(IDR_WOLF3D_BYTE_MAP1), L"WOLF3D_BYTE_MAP");
        if (hResource)
        {
            HGLOBAL hResourceMemory = LoadResource(NULL, hResource);
            if (hResourceMemory)
            {
                BYTE* pResource = (BYTE*)LockResource(hResourceMemory);
                if (pResource)
                {
                    for (int x = 1; x < 65; x++)
                    {
                        for (int y = 1; y < 65; y++)
                        {
                            BYTE b = *pResource;
                            if (b != 0)
                            {
                                AddCubeVertices(x,y, cubeVertices, cubeIndices);
                            }
                            pResource++;
                        }
                    }
                }
            }
        }

        // create cube resource
        hr = spFactory->CreateGeometryResource("w3de1m1", &spResource);
        if (SUCCEEDED(hr))
        {
            hr = spResource->Initialize(GEOMETRY_RESOURCE_FORMAT_POSITION|GEOMETRY_RESOURCE_FORMAT_TEXTURE0, cubeVertices.data(), cubeVertices.size(), cubeIndices.data(), cubeIndices.size());
            spResource->GenerateNormals();
        }

        // return cube resource
        if (SUCCEEDED(hr))
        {
            *ppResource = spResource.detach();
        }
    }

    return hr;
}

HRESULT ResourceFactory::CreateCube(PCSTR name, IGeometryResource** ppResource)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IContentStudioResourceFactory> spFactory;

    // Create a resource factory to use to create the cube geometry
    hr = ResourceFactory::Create(&spFactory);
    if (SUCCEEDED(hr))
    {
        std::vector<GEOMETRY_RESOURCE_VERTEX> cubeVertices;
        std::vector<uint32_t> cubeIndices;
        stde::com_ptr<IGeometryResource> spResource;

        AddCubeVertices(0,0,cubeVertices, cubeIndices);

        // create cube resource
        hr = spFactory->CreateGeometryResource("cube", &spResource);
        if (SUCCEEDED(hr))
        {
            hr = spResource->Initialize(GEOMETRY_RESOURCE_FORMAT_POSITION|GEOMETRY_RESOURCE_FORMAT_TEXTURE0, cubeVertices.data(), cubeVertices.size(), cubeIndices.data(), cubeIndices.size());
            spResource->GenerateNormals();
        }

        // return cube resource
        if (SUCCEEDED(hr))
        {
            *ppResource = spResource.detach();
        }
    }

    return hr;
}

HRESULT ResourceFactory::CreatePlane(PCSTR name, IGeometryResource** ppResource)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IContentStudioResourceFactory> spFactory;

    struct PlaneVertex
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMFLOAT2 t;
    };

    // Create geometry for a simple plane
    PlaneVertex vertices[] = {
        {DirectX::XMFLOAT3(-10.0, 0.0, 10.0),   DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(10.0, 0.0, 10.0),    DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(10.0, 0.0, -10.0),   DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-10.0, 0.0, -10.0),  DirectX::XMFLOAT2(0, 1)},
    };

    uint32 indices[] = {
        0, 2,   1, 
        0, 3,   2, 
    };
    
    // Create a resource factory to use to create the cube geometry
    hr = ResourceFactory::Create(&spFactory);
    if (SUCCEEDED(hr))
    {
        std::vector<GEOMETRY_RESOURCE_VERTEX> planeVertices;
        std::vector<uint32_t> planeIndices;
        stde::com_ptr<IGeometryResource> spResource;

        // copy vertices
        for(size_t i = 0; i < ARRAYSIZE(vertices); i++)
        {
            GEOMETRY_RESOURCE_VERTEX grv;
            grv.Position.x = vertices[i].v.x;
            grv.Position.y = vertices[i].v.y;
            grv.Position.z = vertices[i].v.z;
            grv.TextureCoord0.x = vertices[i].t.x;
            grv.TextureCoord0.y = vertices[i].t.y;
            planeVertices.push_back(grv);
        }

        // copy indices
        for(size_t i = 0; i < ARRAYSIZE(indices); i++)
        {
            planeIndices.push_back(indices[i]);
        }

        // create plane resource
        hr = spFactory->CreateGeometryResource("cube", &spResource);
        if (SUCCEEDED(hr))
        {
            hr = spResource->Initialize(GEOMETRY_RESOURCE_FORMAT_POSITION|GEOMETRY_RESOURCE_FORMAT_TEXTURE0, planeVertices.data(), planeVertices.size(), planeIndices.data(), planeIndices.size());
            spResource->GenerateNormals();
        }

        // return plane resource
        if (SUCCEEDED(hr))
        {
            *ppResource = spResource.detach();
        }
    }

    return hr;
}

HRESULT ResourceFactory::CreateW3DWall(PCSTR name, ITextureResource** ppResource)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IContentStudioResourceFactory> spFactory;
    
    // Create a resource factory to use to create the cube geometry
    hr = ResourceFactory::Create(&spFactory);
    if (SUCCEEDED(hr))
    {
        stde::com_ptr<ITextureResource> spResource;
        hr = spFactory->CreateTextureResource(name, &spResource);

        if (SUCCEEDED(hr))
        {
            // import texture resource
            HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(IDB_WOLF3DWALL_BITMAP), MAKEINTRESOURCE(RT_BITMAP));
            if (hResource)
            {
                HGLOBAL hResourceMemory = LoadResource(NULL, hResource);
                if (hResourceMemory)
                {
                    void* pResource = LockResource(hResourceMemory);
                    if (pResource)
                    {
                        stde::com_ptr<IStream> spStream;
                        DWORD cbResource = SizeofResource(NULL, hResource);
                        DWORD cbWritten = 0;
                        CoreServices::MemoryStream::Create(0, &spStream);

                        BITMAPFILEHEADER bf = {0};
                        bf.bfSize      = sizeof(BITMAPFILEHEADER);
                        bf.bfType      = 0x4D42;
                        bf.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

                        spStream->Write(&bf, sizeof(bf), &cbWritten);
                        spStream->Write(pResource, cbResource, &cbWritten);
                        CoreServices::MemoryStream::Seek(spStream, SEEK_SET, 0);
                        spResource->Load(spStream);
                    }
                }
            }
        }

        // return texture resource
        if (SUCCEEDED(hr))
        {
            *ppResource = spResource.detach();
        }
    }

    return hr;
}