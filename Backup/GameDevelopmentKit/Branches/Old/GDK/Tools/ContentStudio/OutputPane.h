#pragma once

class COutputList : public CRichEditCtrl
{
public:
    COutputList();
    virtual ~COutputList();

protected:
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
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
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
};
