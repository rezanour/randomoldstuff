#pragma once

class CTextureContent : public ITextureContent
{
public:
    CTextureContent();
    virtual ~CTextureContent();

    HRESULT GetTextureName(std::wstring& name);
    HRESULT SetTextureName(std::wstring& name);
    HRESULT GetTextureBuffer(TextureContentBufferDescription& desc);
    HRESULT SetTextureBuffer(TEXTURECONTENT_DDS_HEADER& header, void* pbuffer, DWORD dwSize);
    HRESULT SaveToFile(LPCWSTR filePath);
    HRESULT LoadFromFile(LPCWSTR filePath);
    HRESULT LoadFromBuffer(size_t width, size_t height, DXGI_FORMAT format, size_t rowPitch, size_t slicePitch, BYTE* pixels);
    HRESULT GenerateMipmaps(UINT count);
    HRESULT ConvertToNormalMap(float amplitude);
    HRESULT GetTextureStream(IStream** ppStream);

    static ITextureContentPtr FromFile(LPCWSTR filePath);

private:
    TextureContentBufferDescription textureDesc;
    HRESULT LoadFromScratchImage(DirectX::ScratchImage& scratchImage);
    HRESULT LoadFromDDSStream(IStream* pStream);
    HRESULT LoadFromTCFile(LPCWSTR filePath);
    HRESULT LoadFromDDSFile(LPCWSTR filePath);
    HRESULT LoadFromTGAFile(LPCWSTR filePath);
    HRESULT LoadFromImageFile(LPCWSTR filePath);
};

typedef std::shared_ptr<CTextureContent> TextureContentPtr;