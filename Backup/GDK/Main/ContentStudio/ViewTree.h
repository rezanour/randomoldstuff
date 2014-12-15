#pragma once

class CViewTree : public CTreeCtrl
{
public:
    CViewTree();

protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
    virtual ~CViewTree();
    void SetNewStyle(long lStyleMask, BOOL bSetBits);
    BOOL TransferItem(HTREEITEM hitem, HTREEITEM hNewParent);

private:
    HTREEITEM MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);
    BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);

private:
    CImageList* m_pDragImage;
    BOOL        m_bDragging;
    HTREEITEM   m_hitemDrag;
    HTREEITEM   m_hitemDrop;

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};
