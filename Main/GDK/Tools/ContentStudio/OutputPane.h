#pragma once

class COutputList : public CRichEditCtrl
{
public:
    COutputList();
    virtual ~COutputList();

protected:
    afx_msg void OnContextMenu(_In_ CWnd* pWnd, _In_ CPoint point);
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();

    DECLARE_MESSAGE_MAP()
};

class OutputPane : public CDockablePane
{
public:
    OutputPane();
    void UpdateFonts();

protected:
    CMFCTabCtrl _tabControl;
    COutputList _outputPaneBuild;
    COutputList _outputPaneDebug;
    COutputList _outputPaneFind;

public:
    virtual ~OutputPane();

protected:
    afx_msg int OnCreate(_In_ LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy);

    DECLARE_MESSAGE_MAP()
};
