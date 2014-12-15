#pragma once

void __stdcall DXUTDebugPrintCallback(char* pValue, void* pContext );

void __stdcall LucidDebugCallback(LPCSTR message, void* pContext);

class COutputList : public CListBox
{
public:
    COutputList();

public:
    virtual ~COutputList();

protected:
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();
    afx_msg void OnViewOutput();

    DECLARE_MESSAGE_MAP()
};

class COutputPane : public CDockablePane
{
public:
    COutputPane();

    void UpdateFonts();

protected:
    CMFCTabCtrl m_wndTabs;
    COutputList m_wndOutputBuild;
    COutputList m_wndOutputDebug;
    COutputList m_wndOutputFind;

protected:
    void AdjustHorzScroll(CListBox& wndListBox);

public:
    virtual ~COutputPane();
    void AddDebugText(LPCSTR message);
    void ClearDebugText();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
};
