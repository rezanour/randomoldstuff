#pragma once

class PropertiesPaneToolBar : public CMFCToolBar
{
public:
    virtual void OnUpdateCmdUI(_In_ CFrameWnd* /*pTarget*/, _In_ BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class PropertiesPane : public CDockablePane
{
public:
    PropertiesPane();
    void AdjustLayout();

public:
    void SetVSDotNetLook(_In_ BOOL bSet)
    {
        _propertyGridControl.SetVSDotNetLook(bSet);
        _propertyGridControl.SetGroupNameFullWidth(bSet);
    }

protected:
    CFont _font;
    CComboBox _comboBox;
    PropertiesPaneToolBar _toolBar;
    CMFCPropertyGridCtrl _propertyGridControl;

public:
    virtual ~PropertiesPane();

protected:
    afx_msg int OnCreate(_In_ LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy);
    afx_msg void OnExpandAllProperties();
    afx_msg void OnUpdateExpandAllProperties(_In_ CCmdUI* pCmdUI);
    afx_msg void OnSortProperties();
    afx_msg void OnUpdateSortProperties(_In_ CCmdUI* pCmdUI);
    afx_msg void OnProperties1();
    afx_msg void OnUpdateProperties1(_In_ CCmdUI* pCmdUI);
    afx_msg void OnProperties2();
    afx_msg void OnUpdateProperties2(_In_ CCmdUI* pCmdUI);
    afx_msg void OnSetFocus(_In_ CWnd* pOldWnd);
    afx_msg void OnSettingChange(_In_ UINT uFlags, _In_ LPCTSTR lpszSection);

    DECLARE_MESSAGE_MAP()

    void InitPropList();
    void SetPropListFont();

    int _comboboxHeight;
};
