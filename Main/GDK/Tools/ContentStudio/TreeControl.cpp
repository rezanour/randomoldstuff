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
    _itemDrop(nullptr),
    _parentPane(nullptr)
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

_Use_decl_annotations_
void TreeControl::SetParentPane(ContentPane* parentPane)
{
    _parentPane = parentPane;
}

_Use_decl_annotations_
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

_Use_decl_annotations_
void TreeControl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pNMHDR);
    CPoint      ptAction;

    GetCursorPos(&ptAction);
    ScreenToClient(&ptAction);

    // Collect the selected item and write the name as CF_UNICODETEXT
    HTREEITEM selectedItem = GetSelectedItem();
    CString selectedName = _parentPane->GetContentPath(selectedItem).c_str();

    HGLOBAL globalMemory = nullptr;
    size_t valueLength = (selectedName.GetLength() + 1) * sizeof(WCHAR);

    globalMemory = GlobalAlloc(GHND, valueLength);
    if (globalMemory)
    {
        byte_t* pMem = (byte_t*)GlobalLock(globalMemory);
        if (pMem)
        {
            ZeroMemory(pMem, valueLength);
            memcpy(pMem, selectedName.GetString(), valueLength - sizeof(WCHAR));
            GlobalUnlock(globalMemory);
        }

        _oleDataSource.CacheGlobalData(CF_UNICODETEXT, globalMemory);
        CRect dragRect;
        GetWindowRect(&dragRect);
        _oleDataSource.DoDragDrop(DROPEFFECT_COPY, &dragRect);
    }
    
    LPARAM lparam;
    lparam =  ptAction.y; 
    lparam =  lparam << 16;
    lparam &= ptAction.x;
    
    SendMessage(WM_LBUTTONUP,0,lparam);

    Invalidate();

    *pResult = 0;
}

_Use_decl_annotations_
void TreeControl::OnMouseMove(UINT nFlags, CPoint point)
{
    HTREEITEM hitem;
    UINT      flags = 0;

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

_Use_decl_annotations_
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