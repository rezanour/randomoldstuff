#pragma once

class ContentPane;

class TreeControl : public CTreeCtrl
{
public:
    TreeControl();

protected:
    virtual BOOL OnNotify(_In_ WPARAM wParam, _In_ LPARAM lParam, _In_ LRESULT* pResult);

public:
    virtual ~TreeControl();
    void SetParentPane(_In_ ContentPane* parentPane);

private:
    CImageList* _dragImage;
    BOOL        _dragging;
    HTREEITEM   _itemDrag;
    HTREEITEM   _itemDrop;

    COleDataSource _oleDataSource;
    ContentPane* _parentPane;

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnBegindrag(_In_ NMHDR *pNMHDR, _In_ LRESULT *pResult);
    afx_msg void OnMouseMove(_In_ UINT nFlags, _In_ CPoint point);
    afx_msg void OnLButtonUp(_In_ UINT nFlags, _In_ CPoint point);
};
