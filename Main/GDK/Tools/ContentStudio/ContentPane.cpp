#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ContentPane::ContentPane() :
    _rootItem(nullptr),
    _gameItem(nullptr),
    _contentItem(nullptr)
{
}

ContentPane::~ContentPane()
{
    
}

BEGIN_MESSAGE_MAP(ContentPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_WORLD_OPEN, OnWorldOpen)
//    ON_COMMAND(ID_WORLD_CUT, OnWorldCut)
    ON_COMMAND(ID_WORLD_COPY, OnWorldCopy)
    ON_COMMAND(ID_WORLD_DELETE, OnWorldDelete)
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

_Use_decl_annotations_
int ContentPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create view:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

    if (!_contentPane.Create(dwViewStyle, rectDummy, this, 4))
    {
        TRACE0("Failed to create file view\n");
        return -1;      // fail to create
    }

    // Set Parent Pane
    _contentPane.SetParentPane(this);

    // Load view images:
    _archiveExplorerImages.Create(IDB_ARCHIVE_24, 16, 0, RGB(255, 0, 255));
    _contentPane.SetImageList(&_archiveExplorerImages, TVSIL_NORMAL);

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

std::wstring ContentPane::GetContentPath(HTREEITEM item)
{
    std::wstring contentPath;
    if (_treeItemDataMap.count(item))
    {
        return _treeItemDataMap[item].contentPath;
    }
    return contentPath;
}

_Use_decl_annotations_
void ContentPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

_Use_decl_annotations_
void ContentPane::SetArchiveName(PCWSTR archiveName)
{
    if (archiveName)
    {
        _contentPane.SetItemText(_rootItem, archiveName);
    }
}

void ContentPane::PopulateDefaultItems()
{
    _rootItem = _contentPane.InsertItem(_T("Untitled"), 0, 0);
    _contentPane.SetItemState(_rootItem, TVIS_BOLD, TVIS_BOLD);
    _gameItem = _contentPane.InsertItem(_T("Game"), 0, 0, _rootItem);
    _contentItem = _contentPane.InsertItem(_T("Files"), 0, 0, _rootItem);

    _contentPane.Expand(_rootItem, TVE_EXPAND);
    _contentPane.Expand(_gameItem, TVE_EXPAND);
    _contentPane.Expand(_contentItem, TVE_EXPAND);
}

_Use_decl_annotations_
void ContentPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CTreeCtrl* pWndTree = (CTreeCtrl*) &_contentPane;
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

    ContentPaneTreeData selectedContent;
    if (GetSelectedContent(selectedContent))
    {
        if (selectedContent.contentPath.rfind(L".world") != std::wstring::npos)
        {
            theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_WORLD, point.x, point.y, this, TRUE);
        }
    }
}

void ContentPane::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    int cyTlb = _toolBar.CalcFixedLayout(FALSE, TRUE).cy;

    _toolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    _contentPane.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void ContentPane::OnWorldOpen()
{
    if (_notifyView)
    {
        _notifyView->OnContentSelectionOpened();
    }
}

void ContentPane::OnWorldCut()
{

}

void ContentPane::OnWorldCopy()
{

}

void ContentPane::OnWorldDelete()
{
    OnDeleteItem(_contentPane.GetSelectedItem());
}

void ContentPane::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rectTree;
    _contentPane.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

_Use_decl_annotations_
void ContentPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    _contentPane.SetFocus();
}

void ContentPane::OnChangeVisualStyle()
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

    _contentPane.SetImageList(&_archiveExplorerImages, TVSIL_NORMAL);
}

void ContentPane::Reset()
{
    _contentPane.DeleteAllItems();
    _treeItemDataMap.clear();
    PopulateDefaultItems();
}

_Use_decl_annotations_
void ContentPane::AddContent(ContentPaneTreeData& contentData)
{
    std::vector< std::basic_string<wchar_t> > x;
    std::wstring fullContentPathWithMapping = contentData.contentMapping + L"\\" + contentData.contentPath;
    size_t n = std::split(fullContentPathWithMapping, x, L'\\', true);
    if (n > 0)
    {
        HTREEITEM firstItem = GetItemByName(nullptr, x[0].c_str(), contentData);
        if (firstItem == nullptr)
        {
            firstItem = _contentPane.InsertItem(x[0].c_str(), contentData.isDirectory ? FOLDER_OBJECT_ICON : GENERIC_OBJECT_ICON, contentData.isDirectory ? FOLDER_OBJECT_ICON : GENERIC_OBJECT_ICON, contentData.fileBasedContent ? _contentItem : _gameItem);
        }

        std::wstring accumPath = x[0];
        HTREEITEM prevItem = firstItem;
        for (int i = 1; i < n; ++i)
        {
            accumPath = accumPath + L"\\" + x[i];
            HTREEITEM item = GetItemByName(prevItem, x[i].c_str(), contentData);
            if (x[i] == x[i - 1])
            {
                // Handle strange case where back-to-back named folders can result in incorrect mappings.
                // Example: foo\\foo. The second 'foo' item will never be inserted because the first item named'foo'
                //          was found and assumed to be the same.
                item = nullptr;
            }

            if (item == nullptr)
            {
                if (!contentData.fileBasedContent)
                {
                    contentData.isDirectory = (i < (n - 1));
                }

                item = _contentPane.InsertItem(x[i].c_str(), contentData.isDirectory ? FOLDER_OBJECT_ICON : GENERIC_OBJECT_ICON, contentData.isDirectory ? FOLDER_OBJECT_ICON : GENERIC_OBJECT_ICON, prevItem);
                if (fullContentPathWithMapping == accumPath)
                {
                    // Only add the a contentData for the item that matches the contentData path value
                    _treeItemDataMap[item] = contentData;
                    // If a directory is detected, add a dummy item to give us a + expand marker
                    if (contentData.isDirectory && contentData.fileBasedContent)
                    {
                        _contentPane.InsertItem(L"8==D", GENERIC_OBJECT_ICON, GENERIC_OBJECT_ICON, item);
                    }
                }
            }
            prevItem = item;
        }
    }

    _contentPane.Expand(contentData.fileBasedContent ? _contentItem : _gameItem, TVE_EXPAND);
}

_Use_decl_annotations_
void ContentPane::OnItemExpanding(HTREEITEM hItem)
{
    if (_treeItemDataMap.count(hItem) && !_treeItemDataMap[hItem].expandedOnce)
    {
        _treeItemDataMap[hItem].expandedOnce = true;
        _contentPane.DeleteItem(_contentPane.GetChildItem(hItem)); // Delete the 8==D item
        _notifyView->OnContentExpanded(_treeItemDataMap[hItem]);
    }
}

void ContentPane::OnSelectionChanged()
{
    ContentPaneTreeData selectedContent;

    if (GetSelectedContent(selectedContent))
    {
        if (_notifyView)
        {
            _notifyView->OnContentSelectionChanged(selectedContent);
        }
    }
}

_Use_decl_annotations_
_Success_(return == true)
bool ContentPane::GetSelectedContent(ContentPaneTreeData& content)
{
    HTREEITEM selectedItem = _contentPane.GetSelectedItem();
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

_Use_decl_annotations_
void ContentPane::GetSelectedBSPGeometryAndTextureContent(std::vector<std::wstring>& geometryList, std::vector<std::wstring>& textureList)
{
    geometryList.clear();
    textureList.clear();

    ContentPaneTreeData content;
    if (GetSelectedContent(content) && content.isDirectory)
    {
        std::wstring contentMapping = content.contentMapping;
        std::wstring contentMappingFullPath = content.contentMappingRoot;
        std::wstring root = content.filePath;

        HANDLE hFind;
        WIN32_FIND_DATA FindData;
        std::wstring actualSearch = root + L"\\*.geometry";

        hFind = FindFirstFile(actualSearch.c_str(), &FindData);
        do
        {
            if (hFind == INVALID_HANDLE_VALUE)
            {
                return;
            }

            std::wstring fileName (FindData.cFileName);

            if (fileName == L"." || fileName == L"..")
            {
                continue;
            }

            else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            {
                continue;
            }

            else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }
            else
            {
                // Add file
                ContentPaneTreeData data;
                data.rootFilePath = root;
                data.filePath = root + L"\\" + fileName;
                data.fileBasedContent = true;
                data.isDirectory = false;
                data.expandedOnce = false;
                data.contentMapping = contentMapping;
                data.contentPath = data.filePath.substr(contentMappingFullPath.length() + 1);
                data.contentMappingRoot = contentMappingFullPath;

                // Save the geometry
                geometryList.push_back(data.contentPath);

                // Handle special geometry that has encoded % texture names.
                if (std::IsEncodedTextureContentPath(data.contentPath))
                {
                    textureList.push_back(std::GetTextureContentPathFromGeometryContentPath(data.contentPath));
                }
            }
        } while (FindNextFile(hFind, &FindData));

        FindClose(hFind);

    }
}

_Use_decl_annotations_
void ContentPane::OnKeyDown(NMTVKEYDOWN* ptvkd)
{
    HTREEITEM selectedItem = _contentPane.GetSelectedItem();

    if (selectedItem != NULL)
    {
        switch(ptvkd->wVKey)
        {
        case VK_DELETE:
            OnDeleteItem(selectedItem);
            break;
        default:
            break;
        }
    }
}

_Use_decl_annotations_
void ContentPane::OnDeleteItem(HTREEITEM selectedItem)
{
    // Avoid deleting primary items
    if ((selectedItem == _rootItem) ||
        (selectedItem == _gameItem) ||
        (selectedItem == _contentItem))
    {
        return;
    }

    if (AfxMessageBox(L"Are you sure you want to delete this item?", MB_ICONQUESTION|MB_YESNO) == IDYES)
    {
        // Remove from tree control
        _contentPane.DeleteItem(selectedItem);

        // TODO: Remove from content
    }

    _contentPane.SetFocus();
}

_Use_decl_annotations_
int ContentPane::ResourceObjectTypeToImageIndex(PCWSTR resource)
{
    if (!resource)
    {
        return GENERIC_OBJECT_ICON;
    }

    return GENERIC_OBJECT_ICON;
}

#define MAXTEXTLEN 50

_Use_decl_annotations_
HTREEITEM ContentPane::GetItemByName(HTREEITEM item, PCWSTR name, const ContentPaneTreeData& content)
{
    if (item == nullptr)
    {
        item = (HTREEITEM)_contentPane.GetRootItem();
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

        _contentPane.GetItem(&tvitem);

        // Did we find it?
        if (itemName.compare(szBuffer) == 0)
        {
            if (_treeItemDataMap.count(item))
            {
                ContentPaneTreeData cptd = _treeItemDataMap[item];
                if (!cptd.isDirectory)
                {
                    if (cptd.filePath == content.filePath)
                    {
                        return item;
                    }
                }
                else
                {
                    return item;
                }
            }
            else
            {
                return item;
            }
        }

        // Check whether we have child items.
        if (tvitem.cChildren)
        {
            // Recursively traverse child items.
            HTREEITEM hItemFound = nullptr;
            HTREEITEM hItemChild = nullptr;

            hItemChild = _contentPane.GetChildItem(item);
            hItemFound = GetItemByName(hItemChild, name, content);

            // Did we find it?
            if (hItemFound != nullptr)
            {
                return hItemFound;
            }
        }

        // Go to next sibling item.
        item = _contentPane.GetNextSiblingItem(item);
    }

    // Not found.
    return nullptr;
}

_Use_decl_annotations_
void ContentPane::OnDoubleClickedItem(HTREEITEM item)
{
    UNREFERENCED_PARAMETER(item);
    if (_notifyView)
    {
        _notifyView->OnContentSelectionOpened();
    }
}

_Use_decl_annotations_
BOOL ContentPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);
    NMHDR* plHeader = (NMHDR*)lParam;

    switch(plHeader->code)
    {
    case NM_DBLCLK:
        OnDoubleClickedItem(_contentPane.GetSelectedItem());
        break;
    case TVN_KEYDOWN:
        OnKeyDown((NMTVKEYDOWN*)lParam);
        break;
    case TVN_SELCHANGED:
        OnSelectionChanged();
        break;
    case TVN_ITEMEXPANDING:
        {
            LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
            OnItemExpanding(pnmtv->itemNew.hItem);
        }
        break;
    default:
        break;
    }

    return bRes;
}
