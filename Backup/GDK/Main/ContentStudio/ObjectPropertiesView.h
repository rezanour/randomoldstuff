#pragma once

class CObjectPropertiesViewToolBar : public CMFCToolBar
{
public:
    virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CObjectPropertiesView : public CWnd, IContentStudioPropertiesCallback
{
public:
    CObjectPropertiesView();

    void AdjustLayout();
    void SetDocumentProperties(IContentStudioProperties* pProperties);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioPropertiesCallback
    HRESULT OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps);

public:
    void SetVSDotNetLook(BOOL bSet)
    {
        m_wndPropList.SetVSDotNetLook(bSet);
        m_wndPropList.SetGroupNameFullWidth(bSet);
    }

private:
    HRESULT RefreshPropertiesView(IContentStudioProperties* pProperties);

protected:
    CFont m_fntPropList;
    CComboBox m_wndObjectCombo;
    CObjectPropertiesViewToolBar m_wndToolBar;
    CMFCPropertyGridCtrl m_wndPropList;
    int64              m_propertiesCallbackCookie;
    stde::com_ptr<IContentStudioProperties> m_documentProperties;
    long _refcount;

public:
    virtual ~CObjectPropertiesView();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnExpandAllProperties();
    afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
    afx_msg void OnSortProperties();
    afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
    afx_msg void OnProperties1();
    afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
    afx_msg void OnProperties2();
    afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

    DECLARE_MESSAGE_MAP()

    void InitPropList();
    void SetPropListFont();
};
