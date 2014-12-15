#pragma once

#include <Windows.h>
#include <GDK\Tools\contentstudioproperties.h>

[uuid("18ba31f9-c4e0-42a4-8f3a-570740cedaf7")]
struct IContentStudioWindow : public IUnknown
{
    virtual HRESULT GetCaption(_In_count_(cchCaption) WCHAR* pCaption, _In_ size_t cchCaption) = 0;
    virtual HRESULT Create(_In_ LPCREATESTRUCT lpCreateStruct) = 0;
    virtual void OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy) = 0;
    virtual bool IsOwnedWindow(_In_ HWND hWnd) = 0;
    virtual void OnContextMenu(_In_ HWND hWnd, _In_ LPPOINT pPoint) = 0;
    virtual void OnPaint(_In_ HWND hWnd, _In_ HDC hDC) = 0;
    virtual void OnSetFocus(_In_ HWND hOldWnd) = 0;
    virtual void OnSetDocumentProperties(_In_ IContentStudioProperties* pProperties) = 0;
};

[uuid("8cdffbe1-e980-4c69-846b-5c73b04b2105")]
struct IContentStudioToolbar : public IUnknown
{

};

[uuid("a30ca0d7-9898-45b6-ae1b-581a01f25675")]
struct IContentStudioMenu : public IUnknown
{

};

[uuid("5fc6274a-f7c6-449e-913c-7a1c719e1e95")]
struct IContentStudioDataStore : public IUnknown
{

};

[uuid("9b2dcfab-08fd-41c1-9c86-2ab2e1f58608")]
struct IContentStudioUIExtension : public IUnknown
{
    virtual HRESULT GetWindow(_Deref_out_ IContentStudioWindow** ppWindow) = 0;
    virtual HRESULT GetToolbar(_Deref_out_ IContentStudioToolbar** ppToolbar) = 0;
    virtual HRESULT GetMenu(_Deref_out_ IContentStudioMenu** ppMenu) = 0;
    virtual HRESULT SetDocumentProperties(IContentStudioProperties* pProperties) = 0;
};

const CHAR CreateUIExtensionExport[] = "CreateUIExtension";
typedef HRESULT (__stdcall *PCREATEUIEXTENSION)(LPCWSTR, IContentStudioUIExtension**);