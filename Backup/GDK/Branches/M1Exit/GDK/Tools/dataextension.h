#pragma once

#include <windows.h>

struct IContentStudioResourceFactory;
struct FileExtensionDescription
{
    LPCWSTR extension;
    LPCWSTR description;
};

[uuid("e11c0f26-2511-40bd-8d54-9c3ed46582cb")]
struct IContentStudioDataExtension : public IUnknown
{
    virtual HRESULT GetSupportedFileDescriptions(FileExtensionDescription** ppDescriptions, _Out_ size_t& numDescriptions) = 0;
    virtual HRESULT Import(_In_ LPCWSTR fileName, _In_ IContentStudioResourceFactory* pFactory, _In_ IContentStudioProperties* pProperties) = 0;
    virtual HRESULT Bake(_In_ ITextureResource* pResource, _In_ IStream* pStream) = 0;
    virtual HRESULT Bake(_In_ IGeometryResource* pResource, _In_ IStream* pStream) = 0;
};

const CHAR CreateDataExtensionExport[] = "CreateDataExtension";
typedef HRESULT (__stdcall *PCREATEDATAEXTENSION)(LPCWSTR, IContentStudioDataExtension**);

