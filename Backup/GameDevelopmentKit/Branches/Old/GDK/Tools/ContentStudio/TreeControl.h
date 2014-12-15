#pragma once

class TreeControl : public CTreeCtrl
{
public:
    TreeControl();

protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
    virtual ~TreeControl();

private:
    CImageList* _dragImage;
    BOOL        _dragging;
    HTREEITEM   _itemDrag;
    HTREEITEM   _itemDrop;

    COleDataSource _oleDataSource;

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
