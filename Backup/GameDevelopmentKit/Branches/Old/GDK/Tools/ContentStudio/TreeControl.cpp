#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TreeControl::TreeControl() :
    _dragImage(nullptr),
    _dragging(false),
    _itemDrag(nullptr),
    _itemDrop(nullptr)
{
}

TreeControl::~TreeControl()
{
}

BEGIN_MESSAGE_MAP(TreeControl, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &TreeControl::OnTvnBegindrag)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL TreeControl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

void TreeControl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pNMHDR);
    //LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    CPoint      ptAction;
    //UINT        nFlags;

    GetCursorPos(&ptAction);
    ScreenToClient(&ptAction);

    /*
    _dragging = TRUE;
    _itemDrag = HitTest(ptAction, &nFlags);
    _itemDrop = NULL;

    _dragImage = CreateDragImage(_itemDrag);  // get the image list for dragging
    _dragImage->DragShowNolock(TRUE);
    _dragImage->SetDragCursorImage(0, CPoint(0, 0));
    _dragImage->BeginDrag(0, CPoint(-16,0));
    _dragImage->DragMove(ptAction);
    _dragImage->DragEnter(this, ptAction);
    SetCapture();
    */

    // Collect the selected item and write the name as CF_TEXT
    HGLOBAL globalMemory = nullptr;
    std::string value = "Dookie";
    size_t valueLength = value.length() + 1;

    globalMemory = GlobalAlloc(GHND, valueLength);
    byte_t* pMem = (byte_t*)GlobalLock(globalMemory);
    if (pMem)
    {
        ZeroMemory(pMem, valueLength);
        memcpy(pMem, value.data(), valueLength - sizeof(CHAR));
        GlobalUnlock(globalMemory);
    }

    _oleDataSource.CacheGlobalData(CF_TEXT, globalMemory);
    CRect dragRect;
    GetWindowRect(&dragRect);
    _oleDataSource.DoDragDrop(DROPEFFECT_COPY, &dragRect);

    /*
    LPARAM lparam;
    lparam =  ptAction.y; 
    lparam =  lparam << 16;
    lparam &= ptAction.x;
    
    SendMessage(WM_LBUTTONUP,0,lparam);

    Invalidate();
    */

    *pResult = 0;
}

void TreeControl::OnMouseMove(UINT nFlags, CPoint point)
{
    HTREEITEM hitem;
    UINT      flags;

    if (_dragging)
    {
        _dragImage->DragMove(point);
        if ((hitem = HitTest(point, &flags)) != NULL)
        {
            _dragImage->DragLeave(this);
            SelectDropTarget(hitem);
            _itemDrop = hitem;
            _dragImage->DragEnter(this, point);
        }
    }

    CTreeCtrl::OnMouseMove(nFlags, point);
}

void TreeControl::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_dragging)
    {
        _dragImage->DragLeave(this);
        _dragImage->EndDrag();
        delete _dragImage;
        _dragImage = NULL;

        ReleaseCapture();
        _dragging = FALSE;
        SelectDropTarget(NULL);
    }

    CTreeCtrl::OnLButtonUp(nFlags, point);
}