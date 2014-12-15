#include "stdafx.h"
#include "ContentStudioExtensions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CContentStudioExtensionsApp, CWinApp)
END_MESSAGE_MAP()

CContentStudioExtensionsApp::CContentStudioExtensionsApp()
{

}

CContentStudioExtensionsApp theApp;

BOOL CContentStudioExtensionsApp::InitInstance()
{
    CWinApp::InitInstance();
    return TRUE;
}

extern "C" HRESULT __stdcall EXPORT CreateUIExtension(_In_ LPCWSTR extensionName, _Deref_out_ IContentStudioUIExtension** ppExtension)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    std::wstring eName = extensionName;

    // Create Content Brower Extension
    if (eName == L"Content Browser")
    {
        *ppExtension = new ContentBrowserExtension();
        return S_OK;
    }

    return E_NOTIMPL;
}

extern "C" HRESULT __stdcall EXPORT CreateDataExtension(_In_ LPCWSTR extensionName, _Deref_out_ IContentStudioDataExtension** ppExtension)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    std::wstring eName = extensionName;

    // Create Wavefront Object Model Data Extension
    if (eName == L"Wavefront Model")
    {
        *ppExtension = new WaveFrontModelExtension();
        return S_OK;
    }

    // Create Texture Resource Data Extension
    if (eName == L"Inbox Texture Resource")
    {
        *ppExtension = new TextureResourceExtension();
        return S_OK;
    }

    return E_NOTIMPL;
}
