#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewTree::CViewTree() :
    m_pDragImage(NULL),
    m_bDragging(FALSE),
    m_hitemDrag(NULL),
    m_hitemDrop(NULL)
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CViewTree::OnTvnBegindrag)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewTree::OnTvnEndlabeledit)
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CViewTree::OnTvnBeginlabeledit)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
END_MESSAGE_MAP()

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

    NMHDR* pNMHDR = (NMHDR*)lParam;
    ASSERT(pNMHDR != NULL);

    if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
    {
        GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    return bRes;
}

BOOL CViewTree::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
    do
    {
        if (hitemChild == hitemSuspectedParent)
        break;
    }
    while ((hitemChild = GetParentItem(hitemChild)) != NULL);

    return (hitemChild != NULL);
}

BOOL CViewTree::TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
    TV_INSERTSTRUCT     tvstruct;
    TCHAR               sztBuffer[50];
    HTREEITEM           hNewItem, hFirstChild;

    // avoid an infinite recursion situation
    tvstruct.item.hItem = hitemDrag;
    tvstruct.item.cchTextMax = 49;
    tvstruct.item.pszText = sztBuffer;
    tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    GetItem(&tvstruct.item);  // get information of the dragged element
    tvstruct.hParent = hitemDrop;
    tvstruct.hInsertAfter = TVI_LAST;
    tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    hNewItem = InsertItem(&tvstruct);

    //now copy item data and item state.
    SetItemData(hNewItem,GetItemData(hitemDrag));
    SetItemState(hNewItem,GetItemState(hitemDrag,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);

    while ((hFirstChild = GetChildItem(hitemDrag)) != NULL)
    {
        TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
        DeleteItem(hFirstChild);        // delete the first child and all its children
    }
    return TRUE;
}

void CViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    CPoint      ptAction;
    UINT        nFlags;

    GetCursorPos(&ptAction);
    ScreenToClient(&ptAction);
    m_bDragging = TRUE;
    m_hitemDrag = HitTest(ptAction, &nFlags);
    m_hitemDrop = NULL;

    m_pDragImage = CreateDragImage(m_hitemDrag);  // get the image list for dragging
    m_pDragImage->DragShowNolock(TRUE);
    m_pDragImage->SetDragCursorImage(0, CPoint(0, 0));
    m_pDragImage->BeginDrag(0, CPoint(-16,0));
    m_pDragImage->DragMove(ptAction);
    m_pDragImage->DragEnter(this, ptAction);
    SetCapture();

    *pResult = 0;
}


void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
    HTREEITEM hitem;
    UINT      flags;

    if (m_bDragging)
    {
        m_pDragImage->DragMove(point);
        if ((hitem = HitTest(point, &flags)) != NULL)
        {
            m_pDragImage->DragLeave(this);
            SelectDropTarget(hitem);
            m_hitemDrop = hitem;
            m_pDragImage->DragEnter(this, point);
        }
    }

    CTreeCtrl::OnMouseMove(nFlags, point);
}

HTREEITEM CViewTree::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
    TV_INSERTSTRUCT tvstruct;
    HTREEITEM hNewItem;
    CString sText;

    // get information of the source item
    tvstruct.item.hItem = hItem;
    tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    GetItem(&tvstruct.item);  
    sText = GetItemText( hItem );
        
    tvstruct.item.cchTextMax = sText.GetLength();
    tvstruct.item.pszText = sText.LockBuffer();

    //insert the item at proper location
    tvstruct.hParent = htiNewParent;
    tvstruct.hInsertAfter = htiAfter;
    tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    hNewItem = InsertItem(&tvstruct);
    sText.ReleaseBuffer();

    //now copy item data and item state.
    SetItemData(hNewItem,GetItemData(hItem));
    SetItemState(hNewItem,GetItemState(hItem,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);

    //now delete the old item
    DeleteItem(hItem);

    return hNewItem;
}

void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_pDragImage->DragLeave(this);
        m_pDragImage->EndDrag();
        delete m_pDragImage;
        m_pDragImage = NULL;

        if (m_hitemDrag != m_hitemDrop && !IsChildNodeOf(m_hitemDrop, m_hitemDrag) && GetParentItem(m_hitemDrag) != m_hitemDrop)
        {
            TransferItem(m_hitemDrag, m_hitemDrop);
            DeleteItem(m_hitemDrag);
        }

        ReleaseCapture();
        m_bDragging = FALSE;
        SelectDropTarget(NULL);
    }

    CTreeCtrl::OnLButtonUp(nFlags, point);
}


void CViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

    if (pTVDispInfo->item.pszText != NULL)
    {
        pTVDispInfo->item.mask = TVIF_TEXT;
        SetItem(&pTVDispInfo->item);
    }

    *pResult = 1;
}


void CViewTree::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
}
