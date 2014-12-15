#pragma once

#include <Engine\Renderer.h>

class Lucid3DRenderBufferDesc
{
public:
    VertexBufferPtr     m_vertexBuffer;
    IndexBufferPtr      m_indexBuffer;
    MaterialPtr         m_material;
};

class CLucid3DRenderer
{
public:
    CLucid3DRenderer();
    virtual ~CLucid3DRenderer();

    void Clear();
    void Draw(DeviceContextPtr pContext, IShaderEffect* effect);
    HRESULT AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, IModelContentPtr pModelContent, LPCSTR meshName);
    HRESULT AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, ITextureContentPtr pTextureContent);
    HRESULT AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, ISpriteFontContentPtr pSpriteFontContent);

private:
    HRESULT AddMesh(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, IModelContentPtr pModelContent, MeshInfo meshInfo);
    std::vector<Lucid3DRenderBufferDesc> m_buffers;
};