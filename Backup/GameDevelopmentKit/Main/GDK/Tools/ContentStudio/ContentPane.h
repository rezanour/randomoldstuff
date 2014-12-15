#pragma once

#define GENERIC_OBJECT_ICON 1
#define FOLDER_OBJECT_ICON  0

class ContentPaneToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(_In_ CFrameWnd* /*pTarget*/, _In_ BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class ContentPane : public CDockablePane
{
public:
    ContentPane();

    void AdjustLayout();
    void OnChangeVisualStyle();
    void SetViewNotificationHandler(_In_ INotifyView* view) { _notifyView = view; }
    void SetArchiveName(_In_ PCWSTR archiveName);
    void Reset();
    void AddRequiredResource(_In_ PCWSTR requiredResource);
    _Success_(return == true) bool GetSelectedContent(_Out_ std::wstring& content);

protected:
    TreeControl _archivePane;
    CImageList _archiveExplorerImages;
    ContentPaneToolBar _toolBar;
    HTREEITEM _rootItem;
    HTREEITEM _gameItem;
    HTREEITEM _resourcesItem;

    void PopulateDefaultItems();

    INotifyView* _notifyView;

private:
    HTREEITEM GetItemByName(_In_opt_ HTREEITEM item, _In_ PCWSTR name);
    int ResourceObjectTypeToImageIndex(_In_ PCWSTR resource);

    virtual BOOL OnNotify(_In_ WPARAM wParam, _In_ LPARAM lParam, _Out_ LRESULT* pResult);
    void OnKeyDown(_In_ NMTVKEYDOWN* ptvkd);
    void OnSelectionChanged();
    void OnDeleteItem(_In_ HTREEITEM hSelectedItem);
    void DeleteTreeItemData(_In_ HTREEITEM hItem);
    void OnDoubleClickedItem(_In_ HTREEITEM item);
    std::map<HTREEITEM, std::wstring > _treeItemDataMap;

public:
    virtual ~ContentPane();

protected:
    afx_msg int OnCreate(_In_ LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy);
    afx_msg void OnContextMenu(_In_ CWnd* pWnd, _In_ CPoint point);
    afx_msg void OnImport();
    afx_msg void OnFileOpen();
    afx_msg void OnFileOpenWith();
    afx_msg void OnDummyCompile();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();
    afx_msg void OnPaint();
    afx_msg void OnSetFocus(_In_ CWnd* pOldWnd);

    DECLARE_MESSAGE_MAP()
};
