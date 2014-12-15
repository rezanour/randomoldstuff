#pragma once

#define ID_REFRESH_SOLUTION_EXPLORER 1000

class CContentStudioDoc;

class CSolutionPaneToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CSolutionPane : public CDockablePane
{
public:
    CSolutionPane();

    void AdjustLayout();
    void OnChangeVisualStyle();
    void Update(CContentStudioDoc* pDoc);

protected:

    CViewTree m_wndFileView;
    CImageList m_FileViewImages;
    CSolutionPaneToolBar m_wndToolBar;

protected:
    void EnumerateFiles(CProjectFileNode* pRootNode, HTREEITEM hRoot, BOOL skipRootNode);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    void OnKeyDown(NMTVKEYDOWN* ptvkd);
    void OnSelectionChanged();
    int ImageFromNodeType(ProjectFileNodeType nodeType);
    void OnDeleteItem(HTREEITEM hSelectedItem);

public:
    virtual ~CSolutionPane();

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

