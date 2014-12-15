#include "stdafx.h"

CContentPluginServices::CContentPluginServices()
{
    m_container = std::make_shared<CContentContainer>();
}

CContentPluginServices::~CContentPluginServices()
{

}

IContentContainerPtr CContentPluginServices::GetContentContainer()
{
    return m_container;
}

HRESULT CContentPluginServices::CreateModelContent(IModelContentPtr* ppContent)
{
    HRESULT hr = S_OK;

    *ppContent = std::make_shared<CModelContent>();

    return hr;
}

HRESULT CContentPluginServices::CreateTextureContent(ITextureContentPtr* ppContent)
{
    HRESULT hr = S_OK;

    *ppContent = std::make_shared<CTextureContent>();

    return hr;
}

HRESULT CContentPluginServices::CreateTextureContentFromFile(LPCWSTR filePath, ITextureContentPtr* ppContent)
{
    HRESULT hr = S_OK;

    hr = CreateTextureContent(ppContent);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "no texture content created");

    hr = (*ppContent)->LoadFromFile(filePath);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "no texture content loaded");

Exit:

    if ((FAILED(hr) && *ppContent))
    {
        (*ppContent).reset();
    }

    return hr;
}

/*

// Load an image using WIC and save off a DDS file
            DWORD dxTexFlags = DirectX::WIC_FLAGS_FORCE_RGB;
            DirectX::TexMetadata metadata;
            DirectX::ScratchImage scratchImage;
            hr = DirectX::LoadFromWICFile(szFilePath, dxTexFlags, &metadata, scratchImage);
            if (SUCCEEDED(hr))
            {
                // Generates normal map and saves it
                //DirectX::ScratchImage normalMapImage;
                //hr = DirectX::ComputeNormalMap(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, 0, 10.0f, DXGI_FORMAT_R8G8B8A8_UNORM, normalMapImage);
                //hr = DirectX::SaveToDDSFile(normalMapImage.GetImages(), normalMapImage.GetImageCount(), normalMapImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_RGB | DirectX::DDS_FLAGS_LEGACY_DWORD, L"poop.dds");
                //normalMapImage.Release();

                // Scale to 64x64 and saves it
                //DirectX::ScratchImage scaledImage;
                //hr = DirectX::Resize(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, 64, 64, DirectX::TEX_FILTER_CUBIC, scaledImage);
                //hr = DirectX::SaveToDDSFile(scaledImage.GetImages(), scaledImage.GetImageCount(), scaledImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_RGB | DirectX::DDS_FLAGS_LEGACY_DWORD, L"poop.dds");
                //scaledImage.Release();

                // Generates mipmaps and saves it
                //DirectX::ScratchImage mipMappedImage;
                //hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, DirectX::TEX_FILTER_CUBIC, 10, mipMappedImage);
                //hr = DirectX::SaveToDDSFile(mipMappedImage.GetImages(), mipMappedImage.GetImageCount(), mipMappedImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_RGB | DirectX::DDS_FLAGS_LEGACY_DWORD, L"poop.dds");
                //mipMappedImage.Release();

                // Saves DDS file
                //hr = DirectX::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, DirectX::DDS_FLAGS_FORCE_RGB | DirectX::DDS_FLAGS_LEGACY_DWORD, L"poop.dds");
                //scratchImage.Release();
            }

*/