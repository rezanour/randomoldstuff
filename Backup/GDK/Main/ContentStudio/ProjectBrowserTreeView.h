#pragma once

#include "FileNode.h"

class CProjectBrowserTreeViewToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CProjectBrowserTreeView : public CWnd, IContentStudioPropertiesCallback
{
public:
    CProjectBrowserTreeView();
    virtual ~CProjectBrowserTreeView();

    void AdjustLayout();
    void OnChangeVisualStyle();
    void SetDocumentProperties(IContentStudioProperties* pProperties);
    void Initialize(ProjectBrowseType browseType);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioPropertiesCallback
    HRESULT OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps);



private:
    std::wstring GetTreeItemFilePath(HTREEITEM hItem);

protected:
    CViewTree            m_wndFileView;
    CImageList           m_FileViewImages;
    CProjectBrowserTreeViewToolBar m_wndToolBar;
    int64              m_propertiesCallbackCookie;
    stde::com_ptr<IContentStudioProperties> m_documentProperties;
    long _refcount;
    ProjectBrowseType _browseType;

protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    void OnKeyDown(NMTVKEYDOWN* ptvkd);
    void OnSelectionChanged();
    void OnDeleteItem(HTREEITEM hSelectedItem);
    void DeleteTreeItemData(HTREEITEM hItem);
    std::map<HTREEITEM, stde::com_ptr<IContentStudioProperties> > _treeItemDataMap;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnProperties();
    afx_msg void OnFileOpen();
    afx_msg void OnEditClear();
    afx_msg void OnPaint();
    afx_msg void OnSetFocus(CWnd* pOldWnd);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnExplorerNewfolder();
};

