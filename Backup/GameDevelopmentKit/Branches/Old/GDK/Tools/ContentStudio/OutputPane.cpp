#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

OutputPane::OutputPane()
{
}

OutputPane::~OutputPane()
{
}

BEGIN_MESSAGE_MAP(OutputPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int OutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create tabs window:
    if (!_tabControl.Create(CMFCTabCtrl::STYLE_3D_ONENOTE, rectDummy, this, 1))
    {
        TRACE0("Failed to create output tab window\n");
        return -1;      // fail to create
    }

    // Create output panes:
    const DWORD dwStyle = ES_WANTRETURN | ES_MULTILINE  | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

    if (!_outputPaneBuild.Create(dwStyle, rectDummy, &_tabControl, 2) ||
        !_outputPaneDebug.Create(dwStyle, rectDummy, &_tabControl, 3) ||
        !_outputPaneFind.Create(dwStyle, rectDummy, &_tabControl, 4))
    {
        TRACE0("Failed to create output windows\n");
        return -1;      // fail to create
    }

    UpdateFonts();

    CString strTabName;
    BOOL bNameValid;

    bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
    ASSERT(bNameValid);
    _tabControl.AddTab(&_outputPaneBuild, strTabName, (UINT)0);
    _tabControl.SetTabBkColor(0, RGB(253,251, 172));
    _outputPaneBuild.SetBackgroundColor(FALSE, RGB(253,251, 172));

    bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
    ASSERT(bNameValid);
    _tabControl.AddTab(&_outputPaneDebug, strTabName, (UINT)1);
    _tabControl.SetTabBkColor(1, RGB(153,217, 234));
    _outputPaneDebug.SetBackgroundColor(FALSE, RGB(153,217, 234));

    bNameValid = strTabName.LoadString(IDS_FIND_TAB);
    ASSERT(bNameValid);
    _tabControl.AddTab(&_outputPaneFind, strTabName, (UINT)2);
    _tabControl.SetTabBkColor(2, RGB(152,235, 185));
    _outputPaneFind.SetBackgroundColor(FALSE, RGB(152,235, 185));

    return 0;
}

void OutputPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    _tabControl.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void OutputPane::UpdateFonts()
{
    _outputPaneBuild.SetFont(&afxGlobalData.fontRegular);
    _outputPaneDebug.SetFont(&afxGlobalData.fontRegular);
    _outputPaneFind.SetFont(&afxGlobalData.fontRegular);
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
    Copy();
}

void COutputList::OnEditClear()
{
    this->SetSel(0, -1);
    Clear();
}
