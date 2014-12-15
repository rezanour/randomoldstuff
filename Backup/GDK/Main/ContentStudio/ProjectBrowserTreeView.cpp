#include "stdafx.h"
#include "ProjectBrowserTreeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CProjectBrowserTreeView::CProjectBrowserTreeView() :
    m_propertiesCallbackCookie(0),
    _refcount(1)
{
    
}

CProjectBrowserTreeView::~CProjectBrowserTreeView()
{
    if (m_documentProperties != nullptr)
    {
        m_documentProperties->UnregisterPropertiesCallback(m_propertiesCallbackCookie);
    }
}

BEGIN_MESSAGE_MAP(CProjectBrowserTreeView, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// IUnknown
HRESULT STDMETHODCALLTYPE CProjectBrowserTreeView::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CProjectBrowserTreeView::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE CProjectBrowserTreeView::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        // Let's not delete ourselves on purpose.  See, this object is
        // really owned by another interface and only implements an interface
        // as a requirement to participate in another system.
        // delete this;
    }

    return res;
}

void CProjectBrowserTreeView::Initialize(ProjectBrowseType browseType)
{
    _browseType = browseType;
}

BOOL CProjectBrowserTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);
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

void CProjectBrowserTreeView::OnSelectionChanged()
{
    HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
    if (hSelectedItem != NULL)
    {
        stde::com_ptr<IContentStudioProperties> objectProperties;
        if (_treeItemDataMap[hSelectedItem] != nullptr)
        {
            objectProperties = _treeItemDataMap[hSelectedItem];
            m_documentProperties->SetInterface(SelectedObjectProperty, objectProperties);

            // REMOVE!!!! -- Hacked resource discovery for quick rendering
            //m_documentProperties->SetInterface("SelectedResource", nullptr);

            std::wstring fullPath = ReadStringPropertyW(FullPathProperty, objectProperties);
            if (!fullPath.empty())
            {
                stde::com_ptr<IContentStudioResourceFactory> spFactory;
                stde::com_ptr<IGeometryResource> spGResource;
                stde::com_ptr<ITextureResource> spTResource;

                ResourceFactory::Create(&spFactory);
                
                spFactory->CreateTextureResource("temp", &spTResource);
                if (SUCCEEDED(spTResource->Load(fullPath.c_str())))
                {
                    m_documentProperties->SetInterface("SelectedResource", spTResource);
                }

                spFactory->CreateGeometryResource("temp", &spGResource);
                stde::com_ptr<IStream> spGStream;
                CoreServices::FileStream::Create(fullPath, true, &spGStream);
                if (SUCCEEDED(spGResource->Load(spGStream)))
                {
                    m_documentProperties->SetInterface("SelectedResource", spGResource);
                }
            }

            m_documentProperties->SignalPropertiesChangedEvent();
        }
    }
}

void CProjectBrowserTreeView::OnDeleteItem(HTREEITEM hSelectedItem)
{
    if (AfxMessageBox(_T("Are you sure you want to delete this item?"), MB_YESNO) == IDYES)
    {
        // Delete item
        HTREEITEM hNewSelectedPrevItem = NULL;
        HTREEITEM hNewSelectedNextItem = NULL;
        hNewSelectedPrevItem = m_wndFileView.GetPrevSiblingItem(hSelectedItem);
        hNewSelectedNextItem = m_wndFileView.GetNextSiblingItem(hSelectedItem);

        std::wstring itemFullPath = GetTreeItemFilePath(hSelectedItem);
        if (!itemFullPath.empty())
        {
            DeleteFile(itemFullPath.c_str());
        }

        DeleteTreeItemData(hSelectedItem);
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

void CProjectBrowserTreeView::OnKeyDown(NMTVKEYDOWN* ptvkd)
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

int CProjectBrowserTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
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

void CProjectBrowserTreeView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    AdjustLayout();
}

void CProjectBrowserTreeView::DeleteTreeItemData(HTREEITEM hItem)
{
    if (_treeItemDataMap[hItem] != nullptr)
    {
        // clear the item data cache
        _treeItemDataMap[hItem] = nullptr;
    }
}

void CProjectBrowserTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
    ASSERT_VALID(pWndTree);

    if (pWnd != pWndTree)
    {
        CWnd::OnContextMenu(pWnd, point);
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
    //theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CProjectBrowserTreeView::AdjustLayout()
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

void CProjectBrowserTreeView::OnProperties()
{
    // TODO: switch automatically to properties pane
}

void CProjectBrowserTreeView::OnFileOpen()
{
    // TODO: Force view to load actual model content
}

void CProjectBrowserTreeView::OnEditClear()
{
    OnDeleteItem(m_wndFileView.GetSelectedItem());
}

void CProjectBrowserTreeView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rectTree;
    m_wndFileView.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CProjectBrowserTreeView::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    m_wndFileView.SetFocus();
}

void CProjectBrowserTreeView::OnChangeVisualStyle()
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

void CProjectBrowserTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CWnd::OnLButtonDblClk(nFlags, point);
}

void CProjectBrowserTreeView::OnExplorerNewfolder()
{
    HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
    HTREEITEM hFolder;
    if (hSelectedItem != NULL)
    {
        hFolder = m_wndFileView.InsertItem(L"New Folder", 1, 1, hSelectedItem);
        m_wndFileView.SetItemData(hFolder, (DWORD_PTR)0);
        m_wndFileView.Expand(hSelectedItem, TVE_EXPAND);
        m_wndFileView.EditLabel(hFolder);
    }
}

void CProjectBrowserTreeView::SetDocumentProperties(IContentStudioProperties* pProperties)
{
    if (pProperties == nullptr)
    {
        // revoke registration
        if (m_documentProperties != nullptr)
        {
            m_documentProperties->UnregisterPropertiesCallback(m_propertiesCallbackCookie);
            m_documentProperties = nullptr;
            m_propertiesCallbackCookie = 0;
        }
    }
    else
    {
        m_documentProperties = pProperties;
        pProperties->RegisterPropertiesCallback(this, m_propertiesCallbackCookie);
    }
}

HRESULT CProjectBrowserTreeView::OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps)
{
    bool objectsChanged = false;
    for (size_t i = 0; i < numProps; i++)
    {
        std::string prop = changedProperties[i];
        if (prop == DocumentObjectsProperty)
        {
            objectsChanged = true;
        }
    }

    // rebuild the tree
    if (objectsChanged)
    {
        // Empty file view control
        m_wndFileView.DeleteAllItems();
        stde::com_ptr<IContentStudioPropertiesCollection> documentObjects;

        HTREEITEM hRoot = m_wndFileView.InsertItem(L"Game", 0 /*image id*/, 0 /*image id*/, TVGN_ROOT);
        if (SUCCEEDED(pProperties->GetInterface(DocumentObjectsProperty, (IUnknown**)&documentObjects)))
        {
            size_t numObjects = 0;
            documentObjects->GetTotal(numObjects);
            for (size_t i = 0; i < numObjects; i++)
            {
                stde::com_ptr<IContentStudioProperties> objectProperties;
                documentObjects->Get(i, &objectProperties);
                std::wstring objectName = ReadStringPropertyW(NameProperty, objectProperties);
                std::wstring objectFullPath = ReadStringPropertyW(FullPathProperty, objectProperties);

                if (_browseType == ProjectBrowseType::Assets)
                {
                    // Only enumerate assets
                    if (objectFullPath.rfind(L"asset") != std::wstring::npos)
                    {
                        HTREEITEM hNewItem = m_wndFileView.InsertItem(objectName.c_str(), 4, 4, hRoot);
                        _treeItemDataMap[hNewItem] = objectProperties; 
                    }
                }
                else if (_browseType == ProjectBrowseType::Resources)
                {
                    // Only enumerate resources
                    if ((objectFullPath.rfind(L"resource") != std::wstring::npos))
                    {
                        HTREEITEM hNewItem = m_wndFileView.InsertItem(objectName.c_str(), 4, 4, hRoot);
                        _treeItemDataMap[hNewItem] = objectProperties; 
                    }
                }
                else if (_browseType == ProjectBrowseType::GameObjects)
                {
                    // Only enumerate game objects
                    if (objectFullPath.rfind(L".gameobject") != std::wstring::npos)
                    {
                        HTREEITEM hNewItem = m_wndFileView.InsertItem(objectName.c_str(), 4, 4, hRoot);
                        _treeItemDataMap[hNewItem] = objectProperties; 
                    }
                }
                else if (_browseType == ProjectBrowseType::Components)
                {
                    // Only enumerate components
                    if (objectFullPath.rfind(L"component") != std::wstring::npos)
                    {
                        HTREEITEM hNewItem = m_wndFileView.InsertItem(objectName.c_str(), 4, 4, hRoot);
                        _treeItemDataMap[hNewItem] = objectProperties; 
                    }
                }
                else if (_browseType == ProjectBrowseType::Screens)
                {
                    // Only enumerate screens
                    if (objectFullPath.rfind(L"screen") != std::wstring::npos)
                    {
                        HTREEITEM hNewItem = m_wndFileView.InsertItem(objectName.c_str(), 4, 4, hRoot);
                        _treeItemDataMap[hNewItem] = objectProperties; 
                    }
                }
            }

            hRoot = m_wndFileView.GetNextItem(NULL, TVGN_ROOT);
            m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
            // Expand root and first child to ensure folders are visible
            m_wndFileView.Expand(hRoot, TVE_EXPAND);
        }
    }

    return S_OK;
}

std::wstring CProjectBrowserTreeView::GetTreeItemFilePath(HTREEITEM hItem)
{
    std::wstring path;
    if (hItem != nullptr)
    {
        if (_treeItemDataMap[hItem] != nullptr)
        {
            path = ReadStringPropertyW(FullPathProperty, _treeItemDataMap[hItem]);
        }
    }

    return path;
}
