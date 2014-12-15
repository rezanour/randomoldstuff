#pragma once

enum ProjectBrowseType
{
    Screens,
    GameObjects,
    Assets,
    Resources,
    Components,
};

#include "ViewTree.h"
#include "ProjectBrowserTreeView.h"

class ProjectBrowserWindow : public IContentStudioWindow
{
public:
    ProjectBrowserWindow(ProjectBrowseType browseType);
    virtual ~ProjectBrowserWindow();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioWindow
    HRESULT GetCaption(WCHAR* pCaption, size_t cchCaption);
    HRESULT Create(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, int cx, int cy);
    bool IsOwnedWindow(HWND hWnd);
    void OnContextMenu(HWND hWnd, LPPOINT pPoint);
    void OnPaint(HWND hWnd, HDC hDC);
    void OnSetFocus(HWND hOldWnd);
    void OnSetDocumentProperties(IContentStudioProperties* pProperties);
private:
    long _refcount;
    ProjectBrowseType _browseType;
    HWND _parentWnd;
    CProjectBrowserTreeView _staticWnd;
    stde::com_ptr<IContentStudioProperties> _documentProperties;
};