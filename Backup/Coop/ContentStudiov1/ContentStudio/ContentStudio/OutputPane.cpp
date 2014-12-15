#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutputPane::COutputPane()
{
}

COutputPane::~COutputPane()
{
    DXUTSetDebugPrintCallback(NULL, NULL);
    SET_DEBUG_BROADCAST_CONTEXT(NULL);
}

BEGIN_MESSAGE_MAP(COutputPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create tabs window:
    if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
    {
        TRACE0("Failed to create output tab window\n");
        return -1;      // fail to create
    }

    // Create output panes:
    const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

    if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
        !m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
        !m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
    {
        TRACE0("Failed to create output windows\n");
        return -1;      // fail to create
    }

    UpdateFonts();

    CString strTabName;
    BOOL bNameValid;

    // Attach list windows to tab:
    bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
    ASSERT(bNameValid);
    m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)0);
    bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
    ASSERT(bNameValid);
    m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)1);
    bNameValid = strTabName.LoadString(IDS_FIND_TAB);
    ASSERT(bNameValid);
    m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);

    DXUTSetDebugPrintCallback(DXUTDebugPrintCallback, this);
    SET_DEBUG_BROADCAST_CONTEXT(this);
    SET_DEBUG_BROADCAST(LucidDebugCallback);
    //ENABLE_FUNCTION_TRACING(true);

    return 0;
}

void COutputPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    // Tab control should cover the whole client area:
    m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputPane::AdjustHorzScroll(CListBox& wndListBox)
{
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

    int cxExtentMax = 0;

    for (int i = 0; i < wndListBox.GetCount(); i ++)
    {
        CString strItem;
        wndListBox.GetText(i, strItem);

        cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
    }

    wndListBox.SetHorizontalExtent(cxExtentMax);
    dc.SelectObject(pOldFont);
}

void COutputPane::AddDebugText(LPCSTR pText)
{
    if (m_hWnd != NULL)
    {
        CStringW str;
        str = pText;
        m_wndOutputDebug.AddString(str);
        AdjustHorzScroll(m_wndOutputDebug);
    }
}

void COutputPane::ClearDebugText()
{
    m_wndOutputDebug.ResetContent();
    AdjustHorzScroll(m_wndOutputDebug);
}

void COutputPane::UpdateFonts()
{
    m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
    m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
    m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CMenu menu;
    menu.LoadMenu(IDR_OUTPUT_POPUP);

    CMenu* pSumMenu = menu.GetSubMenu(0);

    if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
    {
        CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

        if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
            return;

        ((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
        UpdateDialogControls(this, FALSE);
    }

    SetFocus();
}

void COutputList::OnEditCopy()
{
    MessageBox(_T("Copy output"));
}

void COutputList::OnEditClear()
{
    //if (g_hackDebugOutput != NULL)
    //{
    //    g_hackDebugOutput->ClearDebugText();
    //}
}

void COutputList::OnViewOutput()
{
    CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
    CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

    if (pMainFrame != NULL && pParentBar != NULL)
    {
        pMainFrame->SetFocus();
        pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
        pMainFrame->RecalcLayout();
    }
}

void __stdcall DXUTDebugPrintCallback(char* pValue, void* pContext)
{
    COutputPane* pPane = (COutputPane*)pContext;
    if (pPane != NULL)
    {
        pPane->AddDebugText((LPCSTR)pValue);
    }
}

void __stdcall LucidDebugCallback(LPCSTR message, void* pContext)
{
    if (pContext)
    {
        COutputPane* pPane = (COutputPane*)pContext;
        pPane->AddDebugText(message);
    }
}
