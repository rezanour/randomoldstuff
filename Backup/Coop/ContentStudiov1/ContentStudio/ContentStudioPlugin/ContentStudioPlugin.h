#pragma once
#pragma warning(disable : 4197) // export 'something' specified multiple times; using first specification
enum SupportedFileType
{
    ftUnknown,
    ftContentStudioModelContent,
    ftWavefrontObj,
    ft3DWorldStudio3DW,
    ftCubePrimitive,
    ftSdkMesh,
    ftDDSTexture,
    ftContentStudioTextureContent,
};

typedef struct _SUPPORTEDFILE
{
    SupportedFileType type; 
    LPCWSTR           extension;
}SUPPORTEDFILE, *PSUPPORTEDFILE;

SupportedFileType FileTypeFromFilePath(LPCWSTR filePath, PSUPPORTEDFILE pSupportedFileArray, UINT numSupportedFiles);
HRESULT LoadTextureContent(LPCWSTR filePath, IContentPluginServices* pServices);