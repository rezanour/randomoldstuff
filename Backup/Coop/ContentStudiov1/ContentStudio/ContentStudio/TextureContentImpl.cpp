#include "stdafx.h"

CTextureContent::CTextureContent()
{

}

CTextureContent::~CTextureContent()
{

}

ITextureContentPtr CTextureContent::FromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    ITextureContentPtr pTextureContent;
    CContentPluginServices* pServices = new CContentPluginServices();
    
    hr = pServices->CreateTextureContent(&pTextureContent);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "Failed to load texture content file '%ws', hr = 0x%lx ", filePath, hr);
        goto Exit;
    }

    hr = pTextureContent->LoadFromFile(filePath);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "Failed to load texture content file '%ws', hr = 0x%lx ", filePath, hr);
        goto Exit;
    }

Exit:

    if FAILED(hr)
    {
        pTextureContent.reset();
    }

    SAFE_DELETE(pServices);

    return pTextureContent;
}

HRESULT CTextureContent::GetTextureBuffer(TextureContentBufferDescription& desc)
{
    desc = textureDesc;

    return S_OK;
}

HRESULT CTextureContent::GetTextureStream(IStream** ppStream)
{
    return E_NOTIMPL;
}

HRESULT CTextureContent::GetTextureName(std::wstring& name)
{
    name = textureDesc.name;
    return S_OK;
}

HRESULT CTextureContent::SetTextureName(std::wstring& name)
{
    textureDesc.name = name;
    return S_OK;
}

HRESULT CTextureContent::SetTextureBuffer(TEXTURECONTENT_DDS_HEADER& header, void* pbuffer, DWORD dwSize)
{
    if (!pbuffer || !dwSize)
    {
        return E_INVALIDARG;
    }

    textureDesc.header = header;
    textureDesc.pBuffer.reset(); // reset any previously stored data
    textureDesc.pBuffer = std::make_shared<CCopiedBuffer>(pbuffer, (size_t)dwSize);

    return S_OK;
}

HRESULT CTextureContent::SaveToFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    DWORD cbWritten = 0;
    IStream* pStream = NULL;

    hr = FileStream::OpenFile(filePath, &pStream, true);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create texture content file");

    DWORD magicNumber = 0x20534444;
    pStream->Write(&magicNumber, sizeof(DWORD), &cbWritten);

    // Write file header
    hr = pStream->Write(&textureDesc.header, sizeof(textureDesc.header), &cbWritten);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to write texture content file header");

    // Write file data
    hr = pStream->Write(textureDesc.pBuffer->ptr, (ULONG)textureDesc.pBuffer->numBytes, &cbWritten);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to write texture content file data");

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CTextureContent::LoadFromDDSStream(IStream* pStream)
{
    HRESULT hr = S_OK;
    DWORD cbRead = 0;
    BYTE* pdata = NULL;
    DWORD cbData = 0;
    STATSTG statstg = {0};
    DWORD magicNumber = 0x20534444;
    DWORD fileMagicNumber = 0;

    hr = pStream->Stat(&statstg, STGTY_STREAM);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get stream size");

    hr = pStream->Read(&fileMagicNumber, sizeof(fileMagicNumber), &cbRead);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to read DDS file magic number");

    if (fileMagicNumber != magicNumber)
    {
        hr = E_FAIL;
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to validate DDS file magic number");
    }

    // Read file header
    hr = pStream->Read(&textureDesc.header, sizeof(textureDesc.header), &cbRead);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to read texture content file header");

    // Read file data
    cbData = statstg.cbSize.LowPart - (sizeof(textureDesc.header) + sizeof(DWORD));
    pdata = new BYTE[cbData];
    hr = pStream->Read(pdata, cbData, &cbRead);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to read texture content file data");

    textureDesc.pBuffer = std::make_shared<CCopiedBuffer>(pdata, cbData);

Exit:

    SAFE_ARRAY_DELETE(pdata);

    return hr;
}

HRESULT CTextureContent::LoadFromTCFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    IStream* pStream = NULL;

    hr = FileStream::OpenFile(filePath, &pStream, false);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load texture content file");

    hr = LoadFromDDSStream(pStream);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load from DDS stream");

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CTextureContent::LoadFromScratchImage(DirectX::ScratchImage& scratchImage)
{
    HRESULT hr = S_OK;
    IStream* pStream = NULL;
    DirectX::Blob blob;

    // Convert any DDS into a DX10 DDS
    hr = DirectX::SaveToDDSMemory(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_DX10_EXT, blob);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load dds file");

    hr = MemoryStream::AttachStream(blob.GetBufferPointer(), blob.GetBufferSize(), &pStream);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create memory stream");

    hr = LoadFromDDSStream(pStream);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed from dds stream");

Exit:

    blob.Release();
    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CTextureContent::LoadFromDDSFile(LPCWSTR filePath)
{
    HRESULT hr = E_FAIL;
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;
    
    hr = DirectX::LoadFromDDSFile(filePath, DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load dds file");

    hr = LoadFromScratchImage(scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load from scratch image");

Exit:

    scratchImage.Release();

    return hr;
}

HRESULT CTextureContent::LoadFromTGAFile(LPCWSTR filePath)
{
    HRESULT hr = E_FAIL;
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;
    
    hr = DirectX::LoadFromTGAFile(filePath, &metadata, scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load tga file");

    hr = LoadFromScratchImage(scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load from scratch image");

Exit:

    scratchImage.Release();

    return hr;
}

HRESULT CTextureContent::LoadFromImageFile(LPCWSTR filePath)
{
    HRESULT hr = E_FAIL;
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;
    
    hr = DirectX::LoadFromWICFile(filePath, DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load WIC file");

    hr = LoadFromScratchImage(scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load from scratch image");

Exit:

    scratchImage.Release();

    return hr;
}

HRESULT CTextureContent::LoadFromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    std::wstring strFilePath = filePath;
    if (strFilePath.rfind(L".tc") != std::wstring::npos)
    {
        hr = LoadFromTCFile(filePath);
    }
    else if (strFilePath.rfind(L".dds") != std::wstring::npos)
    {
        hr = LoadFromDDSFile(filePath);
    }
    else if (strFilePath.rfind(L".tga") != std::wstring::npos)
    {
        hr = LoadFromTGAFile(filePath);
    }
    else
    {
        hr = LoadFromImageFile(filePath);
    }

    return hr;
}

HRESULT CTextureContent::LoadFromBuffer(size_t width, size_t height, DXGI_FORMAT format, size_t rowPitch, size_t slicePitch, BYTE* pixels)
{
    HRESULT hr = S_OK;

    DirectX::Image image = {width, height, format, rowPitch, slicePitch, pixels};
    DirectX::ScratchImage scratchImage;

    if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        scratchImage.InitializeFromImage(image);
    }
    else
    {
        hr = DirectX::Convert(image, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FR_ROTATE0, 1.0f, scratchImage);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to convert buffer to DXGI_FORMAT_R8G8B8A8_UNORM");
    }

    hr = LoadFromScratchImage(scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load buffer from scratch image");

Exit:

    scratchImage.Release();

    return hr;
}

HRESULT CTextureContent::GenerateMipmaps(UINT count)
{
    HRESULT hr = E_FAIL;
    BYTE* pSource = NULL;
    BYTE* pCurrent = NULL;
    DWORD magicNumber = 0x20534444;
    size_t cbSource;
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;
    DirectX::ScratchImage mipmappedImage;

    // Allocate buffer for entire DDS file (including header file magic number)
    cbSource = textureDesc.pBuffer->numBytes + sizeof(textureDesc.header) + sizeof(magicNumber);
    pSource = new BYTE[cbSource];
    pCurrent = pSource;
    
    // write magic number
    memcpy(pCurrent, &magicNumber, sizeof(magicNumber));
    pCurrent += sizeof(magicNumber);

    // write header
    memcpy(pCurrent, &textureDesc.header, sizeof(textureDesc.header));
    pCurrent += sizeof(textureDesc.header);

    // write bits
    memcpy(pCurrent, textureDesc.pBuffer->ptr, textureDesc.pBuffer->numBytes);

    hr = DirectX::LoadFromDDSMemory(pSource, cbSource, DirectX::DDS_FLAGS_FORCE_DX10_EXT | DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load DDS from DDS memory");

    hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_CUBIC, count, mipmappedImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to generate mipmaps from DDS memory");

    // Reset memory
    memset(&textureDesc.header, 0, sizeof(textureDesc.header));
    textureDesc.pBuffer.reset();

    hr = LoadFromScratchImage(mipmappedImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to generate mipmaps from texture content");

Exit:

    scratchImage.Release();
    mipmappedImage.Release();
    SAFE_DELETE_ARRAY(pSource);

    return hr;
}

HRESULT CTextureContent::ConvertToNormalMap(float amplitude)
{
    HRESULT hr = E_FAIL;
    BYTE* pSource = NULL;
    BYTE* pCurrent = NULL;
    DWORD magicNumber = 0x20534444;
    size_t cbSource;
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;
    DirectX::ScratchImage normalmappedImage;

    // Allocate buffer for entire DDS file (including header file magic number)
    cbSource = textureDesc.pBuffer->numBytes + sizeof(textureDesc.header) + sizeof(magicNumber);
    pSource = new BYTE[cbSource];
    pCurrent = pSource;
    
    // write magic number
    memcpy(pCurrent, &magicNumber, sizeof(magicNumber));
    pCurrent += sizeof(magicNumber);

    // write header
    memcpy(pCurrent, &textureDesc.header, sizeof(textureDesc.header));
    pCurrent += sizeof(textureDesc.header);

    // write bits
    memcpy(pCurrent, textureDesc.pBuffer->ptr, textureDesc.pBuffer->numBytes);

    hr = DirectX::LoadFromDDSMemory(pSource, cbSource, DirectX::DDS_FLAGS_FORCE_DX10_EXT | DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load DDS from DDS memory");

    hr = DirectX::ComputeNormalMap(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), 0, amplitude, DXGI_FORMAT_R8G8B8A8_UNORM, normalmappedImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to compute normal map from DDS memory");

    // Reset memory
    memset(&textureDesc.header, 0, sizeof(textureDesc.header));
    textureDesc.pBuffer.reset();

    hr = LoadFromScratchImage(normalmappedImage);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to generate normal map from texture content");

Exit:

    scratchImage.Release();
    normalmappedImage.Release();
    SAFE_DELETE_ARRAY(pSource);

    return hr;
}