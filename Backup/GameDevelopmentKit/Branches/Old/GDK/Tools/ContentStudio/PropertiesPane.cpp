#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PropertiesPane::PropertiesPane()
{
    _comboboxHeight = 0;
}

PropertiesPane::~PropertiesPane()
{
}

BEGIN_MESSAGE_MAP(PropertiesPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
    ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
    ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
    ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
    ON_WM_SETFOCUS()
    ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

void PropertiesPane::AdjustLayout()
{
    if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    int cyTlb = _toolBar.CalcFixedLayout(FALSE, TRUE).cy;

    _comboBox.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), _comboboxHeight, SWP_NOACTIVATE | SWP_NOZORDER);
    _toolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + _comboboxHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    _propertyGridControl.SetWindowPos(NULL, rectClient.left, rectClient.top + _comboboxHeight + cyTlb, rectClient.Width(), rectClient.Height() -(_comboboxHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int PropertiesPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create combo:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (!_comboBox.Create(dwViewStyle, rectDummy, this, 1))
    {
        TRACE0("Failed to create Properties Combo \n");
        return -1;      // fail to create
    }

    _comboBox.AddString(_T("Basic"));
    _comboBox.AddString(_T("Advanced"));
    _comboBox.SetCurSel(0);

    CRect rectCombo;
    _comboBox.GetClientRect (&rectCombo);

    _comboboxHeight = rectCombo.Height();

    if (!_propertyGridControl.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
    {
        TRACE0("Failed to create Properties Grid \n");
        return -1;      // fail to create
    }

    InitPropList();

    _toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
    _toolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
    _toolBar.CleanUpLockedImages();
    _toolBar.LoadBitmap(theApp._hicolorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
    _toolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    _toolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();
    return 0;
}

void PropertiesPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

void PropertiesPane::OnExpandAllProperties()
{
    _propertyGridControl.ExpandAll();
}

void PropertiesPane::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void PropertiesPane::OnSortProperties()
{
    _propertyGridControl.SetAlphabeticMode(!_propertyGridControl.IsAlphabeticMode());
}

void PropertiesPane::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_propertyGridControl.IsAlphabeticMode());
}

void PropertiesPane::OnProperties1()
{

}

void PropertiesPane::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{

}

void PropertiesPane::OnProperties2()
{

}

void PropertiesPane::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{

}

void PropertiesPane::InitPropList()
{
    SetPropListFont();

    _propertyGridControl.EnableHeaderCtrl(FALSE);
    _propertyGridControl.EnableDescriptionArea();
    _propertyGridControl.SetVSDotNetLook();
    _propertyGridControl.MarkModifiedProperties();
}

void PropertiesPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    _propertyGridControl.SetFocus();
}

void PropertiesPane::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CDockablePane::OnSettingChange(uFlags, lpszSection);
    SetPropListFont();
}

void PropertiesPane::SetPropListFont()
{
    ::DeleteObject(_font.Detach());

    LOGFONT lf;
    afxGlobalData.fontRegular.GetLogFont(&lf);

    NONCLIENTMETRICS info;
    info.cbSize = sizeof(info);

    afxGlobalData.GetNonClientMetrics(info);

    lf.lfHeight = info.lfMenuFont.lfHeight;
    lf.lfWeight = info.lfMenuFont.lfWeight;
    lf.lfItalic = info.lfMenuFont.lfItalic;

    _font.CreateFontIndirect(&lf);

    _propertyGridControl.SetFont(&_font);
    _comboBox.SetFont(&_font);
}
