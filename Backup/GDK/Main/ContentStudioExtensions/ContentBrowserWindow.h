#pragma once

class CBrowserWindowToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CBrowserWindow : public CWnd
{
public:
    CBrowserWindow();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
    CBrowserWindowToolBar m_wndToolBar;
};

class ContentBrowserWindow : public IContentStudioWindow
{
public:
    ContentBrowserWindow();
    virtual ~ContentBrowserWindow();

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
    HWND _parentWnd;
    CWnd _staticWnd;
};