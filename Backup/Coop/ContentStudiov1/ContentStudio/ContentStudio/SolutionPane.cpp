#include "stdafx.h"
#include "SolutionPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSolutionPane::CSolutionPane()
{
}

CSolutionPane::~CSolutionPane()
{
}

BEGIN_MESSAGE_MAP(CSolutionPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_PROPERTIES, OnProperties)
    ON_COMMAND(ID_OPEN, OnFileOpen)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(ID_EXPLORER_NEWFOLDER, &CSolutionPane::OnExplorerNewfolder)
END_MESSAGE_MAP()

BOOL CSolutionPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    BOOL bRes = CDockablePane::OnNotify(wParam, lParam, pResult);
    NMHDR* plHeader = (NMHDR*)lParam;

    switch(plHeader->code)
    {
    case TVN_KEYDOWN:
        OnKeyDown((NMTVKEYDOWN*)lParam);
        break;
    case TVN_SELCHANGED:
        OnSelectionChanged();
        break;
    default:
        break;
    }

    return bRes;
}

void CSolutionPane::OnSelectionChanged()
{
    HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
    if (hSelectedItem != NULL)
    {
        // Refresh Properties window if needed
        CMainFrame* pFrame = (CMainFrame*)this->GetParentFrame();
        if (pFrame != NULL)
        {
            CContentStudioView* pView = pFrame->GetView();
            if (pView != NULL)
            {
                CContentStudioDoc* pDoc = pView->GetDocument();
                if (pDoc != NULL)
                {
                    // Set currently selected mesh/model
                    CProjectFileNode* pNode = NULL;
                    pNode = (CProjectFileNode*)m_wndFileView.GetItemData(hSelectedItem);
                    if (pNode != NULL)
                    {
                        pDoc->UpdateCurrentContent(pNode->backingFile.c_str(), pNode->contentIndex);
                        //DEBUG_PRINT("Selected Mesh = %ws", pNode->name.c_str());
                    }
                    pDoc->UpdateAllViews(NULL, ID_REFRESH_MAIN_VIEW);
                }
            }
        }
        int i = 0;
    }
}

void CSolutionPane::OnDeleteItem(HTREEITEM hSelectedItem)
{
    if (AfxMessageBox(_T("Are you sure you want to delete this item?"), MB_YESNO) == IDYES)
    {
        // Delete item
        HTREEITEM hNewSelectedPrevItem = NULL;
        HTREEITEM hNewSelectedNextItem = NULL;
        hNewSelectedPrevItem = m_wndFileView.GetPrevSiblingItem(hSelectedItem);
        hNewSelectedNextItem = m_wndFileView.GetNextSiblingItem(hSelectedItem);

        m_wndFileView.DeleteItem(hSelectedItem);
        if (hNewSelectedPrevItem != NULL)
        {
            m_wndFileView.SelectItem(hNewSelectedPrevItem);
        }
        else if (hNewSelectedNextItem != NULL)
        {
            m_wndFileView.SelectItem(hNewSelectedNextItem);
        }
    }

    SetFocus();
}

void CSolutionPane::OnKeyDown(NMTVKEYDOWN* ptvkd)
{
    HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
    if (hSelectedItem != NULL)
    {
        switch(ptvkd->wVKey)
        {
        case VK_DELETE:
            OnDeleteItem(hSelectedItem);
            break;
        //case VK_ESCAPE:
        //    m_wndFileView.EndEditLabelNow(TRUE);
        //    break;
        default:
            break;
        }
    }
}

int CSolutionPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create view:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_EDITLABELS | TVS_SHOWSELALWAYS;

    if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
    {
        TRACE0("Failed to create file view\n");
        return -1;      // fail to create
    }

    // Load view images:
    m_FileViewImages.Create(IDB_SOLUTION, 16, 0, RGB(255, 0, 255));
    m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

    m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
    m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

    OnChangeVisualStyle();

    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

    m_wndToolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();

    return 0;
}

void CSolutionPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

void CSolutionPane::Update(CContentStudioDoc* pDoc)
{
    CProjectFile* pProjectFile = &pDoc->m_ProjectFile;
    CProjectFileNode* pRootNode = &pProjectFile->nodes;

    // Empty file view control
    m_wndFileView.DeleteAllItems();

    if (pRootNode->children.size() > 0)
    {
        HTREEITEM hRoot = NULL;
        EnumerateFiles(pRootNode, hRoot, TRUE);

        hRoot = m_wndFileView.GetNextItem(NULL, TVGN_ROOT);
        pRootNode->hTreeItem = hRoot;
        m_wndFileView.SetItemData(pRootNode->hTreeItem, (DWORD_PTR)pRootNode);

        m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
        
        // Expand root and first child to ensure folders are visible
        m_wndFileView.Expand(hRoot, TVE_EXPAND);
        //m_wndFileView.Expand(m_wndFileView.GetNextItem(hRoot, TVGN_CHILD), TVE_EXPAND);
    }
}

int CSolutionPane::ImageFromNodeType(ProjectFileNodeType nodeType)
{
    return (int)nodeType;
}

void CSolutionPane::EnumerateFiles(CProjectFileNode* pRootNode, HTREEITEM hRoot, BOOL skipRootNode)
{
    if (pRootNode != NULL)
    {
        // Node is a folder and has children, iterate them and recurse
        if (pRootNode->children.size() > 0)
        {
            HTREEITEM hFolder = hRoot;

            // Insert Folder node, if a valid node name is present, otherwise, use
            // the existing root as the folder node.
            if ((pRootNode->name.length() != 0) && (!skipRootNode))
            {
                hFolder = m_wndFileView.InsertItem(pRootNode->name.c_str(), ImageFromNodeType(pRootNode->nodeType), ImageFromNodeType(pRootNode->nodeType), hRoot);
            }

            // Set links into backing data from control
            pRootNode->hTreeItem = hFolder;
            m_wndFileView.SetItemData(pRootNode->hTreeItem, (DWORD_PTR)pRootNode);

            // Iterate over childern
            std::vector<CProjectFileNode*>::iterator fileNodeIterator;
            for(fileNodeIterator = pRootNode->children.begin();  fileNodeIterator != pRootNode->children.end(); fileNodeIterator++)
            {
                // Recurse for each child found
                EnumerateFiles(*fileNodeIterator, hFolder, FALSE);
            }
        }
        else
        {
            // Node is a file
            if ((pRootNode->name.length() > 0) && (!skipRootNode))
            {
                pRootNode->hTreeItem = m_wndFileView.InsertItem(pRootNode->name.c_str(), ImageFromNodeType(pRootNode->nodeType), ImageFromNodeType(pRootNode->nodeType), hRoot);
                m_wndFileView.SetItemData(pRootNode->hTreeItem, (DWORD_PTR)pRootNode);
            }
        }
    }
}

void CSolutionPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
    ASSERT_VALID(pWndTree);

    if (pWnd != pWndTree)
    {
        CDockablePane::OnContextMenu(pWnd, point);
        return;
    }

    if (point != CPoint(-1, -1))
    {
        // Select clicked item:
        CPoint ptTree = point;
        pWndTree->ScreenToClient(&ptTree);

        UINT flags = 0;
        HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
        if (hTreeItem != NULL)
        {
            pWndTree->SelectItem(hTreeItem);
        }
    }

    pWndTree->SetFocus();
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CSolutionPane::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

    m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CSolutionPane::OnProperties()
{
    // TODO: switch automatically to properties pane
}

void CSolutionPane::OnFileOpen()
{
    // TODO: Force view to load actual model content
}

void CSolutionPane::OnEditClear()
{
    OnDeleteItem(m_wndFileView.GetSelectedItem());
}

void CSolutionPane::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rectTree;
    m_wndFileView.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CSolutionPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    m_wndFileView.SetFocus();
}

void CSolutionPane::OnChangeVisualStyle()
{
    m_wndToolBar.CleanUpLockedImages();
    m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

    m_FileViewImages.DeleteImageList();

    UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_SOLUTION_24 : IDB_SOLUTION;

    CBitmap bmp;
    if (!bmp.LoadBitmap(uiBmpId))
    {
        TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
        ASSERT(FALSE);
        return;
    }

    BITMAP bmpObj;
    bmp.GetBitmap(&bmpObj);

    UINT nFlags = ILC_MASK;

    nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

    m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
    m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

    m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

void CSolutionPane::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CDockablePane::OnLButtonDblClk(nFlags, point);
}

void CSolutionPane::OnExplorerNewfolder()
{
    HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
    HTREEITEM hFolder;
    if (hSelectedItem != NULL)
    {
        hFolder = m_wndFileView.InsertItem(L"New Folder", ImageFromNodeType(efolder), ImageFromNodeType(efolder), hSelectedItem);
        m_wndFileView.SetItemData(hFolder, (DWORD_PTR)0);
        m_wndFileView.Expand(hSelectedItem, TVE_EXPAND);
        m_wndFileView.EditLabel(hFolder);
    }
}
