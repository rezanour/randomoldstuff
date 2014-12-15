#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ArchiveExplorerPane::ArchiveExplorerPane() :
    _rootItem(nullptr),
    _gameItem(nullptr),
    _resourcesItem(nullptr)
{
}

ArchiveExplorerPane::~ArchiveExplorerPane()
{
    
}

BEGIN_MESSAGE_MAP(ArchiveExplorerPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_IMPORT, OnImport)
    ON_COMMAND(ID_OPEN, OnFileOpen)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int ArchiveExplorerPane::OnCreate(_In_ LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create view:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

    if (!_archivePane.Create(dwViewStyle, rectDummy, this, 4))
    {
        TRACE0("Failed to create file view\n");
        return -1;      // fail to create
    }

    // Load view images:
    _archiveExplorerImages.Create(IDB_ARCHIVE_24, 16, 0, RGB(255, 0, 255));
    _archivePane.SetImageList(&_archiveExplorerImages, TVSIL_NORMAL);

    _toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
    _toolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

    OnChangeVisualStyle();

    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

    _toolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    _toolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();

    return 0;
}

void ArchiveExplorerPane::OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

void ArchiveExplorerPane::SetArchiveName(_In_ PCWSTR archiveName)
{
    if (archiveName)
    {
        _archivePane.SetItemText(_rootItem, archiveName);
    }
}

void ArchiveExplorerPane::PopulateDefaultItems()
{
    _rootItem = _archivePane.InsertItem(_T("Untitled"), 0, 0);
    _archivePane.SetItemState(_rootItem, TVIS_BOLD, TVIS_BOLD);
    _gameItem = _archivePane.InsertItem(_T("Game"), 0, 0, _rootItem);
    _resourcesItem = _archivePane.InsertItem(_T("Resources"), 0, 0, _rootItem);

    _archivePane.Expand(_rootItem, TVE_EXPAND);
    _archivePane.Expand(_gameItem, TVE_EXPAND);
    _archivePane.Expand(_resourcesItem, TVE_EXPAND);
}

void ArchiveExplorerPane::OnContextMenu(_In_ CWnd* pWnd, _In_ CPoint point)
{
    CTreeCtrl* pWndTree = (CTreeCtrl*) &_archivePane;
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

void ArchiveExplorerPane::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    int cyTlb = _toolBar.CalcFixedLayout(FALSE, TRUE).cy;

    _toolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    _archivePane.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void ArchiveExplorerPane::OnImport()
{
    AfxMessageBox(_T("Import...."));
}

void ArchiveExplorerPane::OnFileOpen()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnFileOpenWith()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnDummyCompile()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnEditCut()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnEditCopy()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnEditClear()
{
    // TODO: Add your command handler code here
}

void ArchiveExplorerPane::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rectTree;
    _archivePane.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void ArchiveExplorerPane::OnSetFocus(_In_ CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    _archivePane.SetFocus();
}

void ArchiveExplorerPane::OnChangeVisualStyle()
{
    _toolBar.CleanUpLockedImages();
    _toolBar.LoadBitmap(IDB_EXPLORER_24, 0, 0, TRUE /* Locked */);

    _archiveExplorerImages.DeleteImageList();

    UINT uiBmpId = IDB_ARCHIVE_24;

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

    nFlags |= (theApp._hicolorIcons) ? ILC_COLOR24 : ILC_COLOR4;

    _archiveExplorerImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
    _archiveExplorerImages.Add(&bmp, RGB(255, 0, 255));

    _archivePane.SetImageList(&_archiveExplorerImages, TVSIL_NORMAL);
}

void ArchiveExplorerPane::Reset()
{
    _archivePane.DeleteAllItems();
    _treeItemDataMap.clear();
    PopulateDefaultItems();
}

void ArchiveExplorerPane::AddRequiredResource(_In_ PCWSTR requiredResource)
{
    int iconImage = GENERIC_OBJECT_ICON;
    std::wstring parseResource = requiredResource;
    std::vector< std::basic_string<wchar_t> > x;
    size_t n = std::split(parseResource, x, L'\\', true);
    if (n > 0)
    {
        // Match first item
        HTREEITEM firstItem = GetItemByName(nullptr, x[0].c_str());
        HTREEITEM secondItem = nullptr;
        HTREEITEM thirdItem = nullptr;
        if (firstItem == nullptr)
        {
            firstItem = _archivePane.InsertItem(x[0].c_str(), FOLDER_OBJECT_ICON, FOLDER_OBJECT_ICON, _gameItem);
        }
        
        secondItem = GetItemByName(firstItem, x[1].c_str());
        if (secondItem == nullptr)
        {
            iconImage = ResourceObjectTypeToImageIndex(x[1].c_str());
            secondItem = _archivePane.InsertItem(x[1].c_str(), iconImage, iconImage, firstItem);
            _treeItemDataMap[secondItem] = x[0] + L"\\" + x[1];
        }

        if (x.size() > 2)
        {
            thirdItem = GetItemByName(secondItem, x[2].c_str());
            if (thirdItem == nullptr)
            {
                iconImage = ResourceObjectTypeToImageIndex(x[2].c_str());
                thirdItem = _archivePane.InsertItem(x[2].c_str(), iconImage, iconImage, secondItem);                                
                _treeItemDataMap[thirdItem] = requiredResource;
            }
        }
        
        _archivePane.Expand(_gameItem, TVE_EXPAND);
    }
}

void ArchiveExplorerPane::OnSelectionChanged()
{
    std::wstring selectedContent;

    if (GetSelectedContent(selectedContent))
    {
        if (_notifyView)
        {
            _notifyView->OnContentSelectionChanged();
        }
    }
}

bool ArchiveExplorerPane::GetSelectedContent(_Out_ std::wstring& content)
{
    HTREEITEM selectedItem = _archivePane.GetSelectedItem();
    if (selectedItem != NULL)
    {
        if (_treeItemDataMap.count(selectedItem))
        {
            content = _treeItemDataMap[selectedItem];
            return true;
        }
    }

    return false;
}

void ArchiveExplorerPane::OnKeyDown(_In_ NMTVKEYDOWN* ptvkd)
{
    HTREEITEM selectedItem = _archivePane.GetSelectedItem();
    if (selectedItem != NULL)
    {
        switch(ptvkd->wVKey)
        {
        case VK_DELETE:
            OnDeleteItem(selectedItem);
            break;

        case VK_F1:
            _notifyView->OnContentFrameChanged(false);
            break;

        case VK_F2:
            _notifyView->OnContentFrameChanged(true);
            break;

        default:
            break;
        }
    }
}

void ArchiveExplorerPane::OnDeleteItem(_In_ HTREEITEM selectedItem)
{
    UNREFERENCED_PARAMETER(selectedItem);
}

int ArchiveExplorerPane::ResourceObjectTypeToImageIndex(_In_ PCWSTR resource)
{
    if (!resource)
    {
        return GENERIC_OBJECT_ICON;
    }

    return GENERIC_OBJECT_ICON;
}

#define MAXTEXTLEN 50

HTREEITEM ArchiveExplorerPane::GetItemByName(_In_ HTREEITEM item, _In_ PCWSTR name)
{
    if (item == nullptr)
    {
        item = (HTREEITEM)_archivePane.GetRootItem();
    }
    
    std::wstring itemName = name;

    while (item != NULL)
    {
        wchar_t szBuffer[MAXTEXTLEN+1];
        TV_ITEM tvitem;

        tvitem.hItem = item;
        tvitem.mask = TVIF_TEXT | TVIF_CHILDREN;
        tvitem.pszText = szBuffer;
        tvitem.cchTextMax = MAXTEXTLEN;

        _archivePane.GetItem(&tvitem);

        // Did we find it?
        if (itemName.compare(szBuffer) == 0)
            return item;

        // Check whether we have child items.
        if (tvitem.cChildren)
        {
            // Recursively traverse child items.
            HTREEITEM hItemFound = nullptr;
            HTREEITEM hItemChild = nullptr;

            hItemChild = _archivePane.GetChildItem(item);
            hItemFound = GetItemByName(hItemChild, name);

            // Did we find it?
            if (hItemFound != nullptr)
                return hItemFound;
        }

        // Go to next sibling item.
        item = _archivePane.GetNextSiblingItem(item);
    }

    // Not found.
    return nullptr;
}

void ArchiveExplorerPane::OnDoubleClickedItem(_In_ HTREEITEM item)
{
    UNREFERENCED_PARAMETER(item);
    if (_notifyView)
    {
        _notifyView->OnContentSelectionOpened();
    }
}

BOOL ArchiveExplorerPane::OnNotify(_In_ WPARAM wParam, _In_ LPARAM lParam, _Out_ LRESULT* pResult)
{
    BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);
    NMHDR* plHeader = (NMHDR*)lParam;

    switch(plHeader->code)
    {
    case NM_DBLCLK:
        OnDoubleClickedItem(_archivePane.GetSelectedItem());
        break;
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
