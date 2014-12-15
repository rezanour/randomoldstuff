#include "stdafx.h"
#include "ContentStudioPlugin.h"

SUPPORTEDFILE g_supportedLoadExtensions[] = {
    {ftContentStudioModelContent,   L".mc"},
    {ftWavefrontObj,                L".obj"},
    {ft3DWorldStudio3DW,            L".3dw"},
    {ftCubePrimitive,               L".cube"},
    {ftSdkMesh,                     L".sdkmesh"},
    {ftContentStudioTextureContent, L".tc"},
    {ftDDSTexture,                  L".dds"},
    {ftDDSTexture,                  L".jpg"},
    {ftDDSTexture,                  L".tif"},
    {ftDDSTexture,                  L".tiff"},
    {ftDDSTexture,                  L".png"},
    {ftDDSTexture,                  L".bmp"},
    {ftDDSTexture,                  L".gif"},
    {ftDDSTexture,                  L".tga"},
};

SUPPORTEDFILE g_supportedSaveExtensions[] = {
    {ftContentStudioModelContent,   L".mc"},
    {ftContentStudioTextureContent, L".tc"},
    {ftDDSTexture,                  L".dds"},
};

// Returns TRUE if the specified file path contains a supported extension. (IMPORT)
__declspec(dllexport) BOOL __stdcall CanLoadContent(LPCWSTR filePath)
{
    return (FileTypeFromFilePath(filePath, g_supportedLoadExtensions, ARRAYSIZE(g_supportedLoadExtensions)) != ftUnknown);
}

// Returns TRUE if the specified file path contains a supported extension. (EXPORT)
__declspec(dllexport) BOOL __stdcall CanSaveContent(LPCWSTR filePath)
{
    return (FileTypeFromFilePath(filePath, g_supportedSaveExtensions, ARRAYSIZE(g_supportedSaveExtensions)) != ftUnknown);
}

// Loads content from the specified file. (IMPORT)
__declspec(dllexport) HRESULT __stdcall LoadContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    HRESULT hr = S_OK;

    // Double check incoming file path against our supported set.  Assume that the caller may not have checked
    // before calling this entry point.
    if (!CanLoadContent(filePath))
    {
        return E_NOTIMPL;
    }

    // Get container to load content into
    IContentContainerPtr pContainer = pServices->GetContentContainer();

    SupportedFileType fileType = FileTypeFromFilePath(filePath, g_supportedLoadExtensions, ARRAYSIZE(g_supportedLoadExtensions));
    switch(fileType)
    {
    case ftContentStudioModelContent:
        hr = LoadMCContent(filePath, pServices);
        break;
    case ftWavefrontObj:
        hr = LoadObjContent(filePath, pServices);
        break;
    case ft3DWorldStudio3DW:
        hr = Load3dwContent(filePath, pServices);
        break;
    case ftCubePrimitive:
        hr = LoadCubePrimitiveContent(filePath, pServices);
        break;
    case ftSdkMesh:
        hr = LoadSdkMeshContent(filePath, pServices);
        break;
    case ftDDSTexture:
        hr = LoadTextureContent(filePath, pServices);
        break;
    case ftContentStudioTextureContent:
        hr = LoadTextureContent(filePath, pServices);
        break;
    case ftUnknown:
    default:
        hr = E_NOTIMPL;
        break;
    }

    return hr;
}

// Saves content to the specified file. (EXPORT)
__declspec(dllexport) HRESULT __stdcall SaveContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    HRESULT hr = S_OK;

    // Double check incoming file path against our supported set.  Assume that the caller may not have checked
    // before calling this entry point.
    if (!CanSaveContent(filePath))
    {
        return E_NOTIMPL;
    }

    // Get container to save content from
    IContentContainerPtr pContainer = pServices->GetContentContainer();

    SupportedFileType fileType = FileTypeFromFilePath(filePath, g_supportedSaveExtensions, ARRAYSIZE(g_supportedSaveExtensions));
    switch(fileType)
    {
    case ftContentStudioModelContent:
        hr = SaveMCContent(filePath, pServices);
        break;
    case ftDDSTexture:
    case ftContentStudioTextureContent:
    case ftWavefrontObj:
    case ft3DWorldStudio3DW:
    case ftUnknown:
    default:
        hr = E_NOTIMPL;
        break;
    }

    return hr;
}

// Enumerate supported file types and return the proper enumeration representing that type.  If no type
// is found, ftUnknown will be returned.
SupportedFileType FileTypeFromFilePath(LPCWSTR filePath, PSUPPORTEDFILE pSupportedFileArray, UINT numSupportedFiles)
{
    SupportedFileType supportedType = ftUnknown;

    std::wstring strFilePath = filePath;
    for (UINT i = 0; i < numSupportedFiles; i++)
    {
        if (strFilePath.rfind(pSupportedFileArray[i].extension) != std::wstring::npos)
        {
            supportedType = pSupportedFileArray[i].type;
            break;
        }
    }

    return supportedType;
}

std::wstring FileFromFilePath(LPCWSTR szFilePath, BOOL includeExtensionInName)
{
    // Trim filename from full path
    std::wstring filePath = szFilePath;
    size_t pos = filePath.find_last_of(L"\\");
    std::wstring fileName = filePath.substr(pos + 1);
    
    // Trim filename without extension
    pos = fileName.find_last_of(L".");
    std::wstring name = fileName.substr(0, pos);

    if (includeExtensionInName)
    {
        return fileName;
    }
    else
    {
        return name;
    }
}

HRESULT LoadTextureContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Loading Texture content...");

    HRESULT hr = S_OK;
    ITextureContentPtr pTextureContent;

    hr = pServices->CreateTextureContent(&pTextureContent);
    if (SUCCEEDED(hr))
    {
        hr = pTextureContent->LoadFromFile(filePath);
        if (SUCCEEDED(hr))
        {
            pTextureContent->SetTextureName(FileFromFilePath(filePath, FALSE));
            IContentContainerPtr pContent = pServices->GetContentContainer();
            if (pContent)
            {
                hr = pContent->AddTextureContent(pTextureContent);
            }
        }
    }
    return hr;
}