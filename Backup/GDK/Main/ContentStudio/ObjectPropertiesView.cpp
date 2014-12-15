#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CObjectPropertiesView::CObjectPropertiesView() :
    m_propertiesCallbackCookie(0),
    _refcount(1)
{
}

CObjectPropertiesView::~CObjectPropertiesView()
{
    if (m_documentProperties != nullptr)
    {
        m_documentProperties->UnregisterPropertiesCallback(m_propertiesCallbackCookie);
    }
}

BEGIN_MESSAGE_MAP(CObjectPropertiesView, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
    ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
    ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
    ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
    ON_COMMAND(ID_PROPERTIES1, OnProperties1)
    ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
    ON_COMMAND(ID_PROPERTIES2, OnProperties2)
    ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
    ON_WM_SETFOCUS()
    ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

// IUnknown
HRESULT STDMETHODCALLTYPE CObjectPropertiesView::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE CObjectPropertiesView::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE CObjectPropertiesView::Release()
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

void CObjectPropertiesView::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient,rectCombo;
    GetClientRect(rectClient);

    m_wndObjectCombo.GetWindowRect(&rectCombo);

    int cyCmb = rectCombo.Size().cy;
    int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

    m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CObjectPropertiesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create combo:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
    {
        TRACE0("Failed to create Properties Combo \n");
        return -1;      // fail to create
    }

    //m_wndObjectCombo.AddString(_T("Application"));
    m_wndObjectCombo.AddString(_T("Properties Window"));
    m_wndObjectCombo.SetCurSel(0);

    if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
    {
        TRACE0("Failed to create Properties Grid \n");
        return -1;      // fail to create
    }

    InitPropList();

    m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
    m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
    m_wndToolBar.CleanUpLockedImages();
    m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
    m_wndToolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();
    return 0;
}

void CObjectPropertiesView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    AdjustLayout();
}

void CObjectPropertiesView::OnExpandAllProperties()
{
    m_wndPropList.ExpandAll();
}

void CObjectPropertiesView::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CObjectPropertiesView::OnSortProperties()
{
    m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CObjectPropertiesView::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CObjectPropertiesView::OnProperties1()
{
    // TODO: Add your command handler code here
}

void CObjectPropertiesView::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
    // TODO: Add your command update UI handler code here
}

void CObjectPropertiesView::OnProperties2()
{
    // TODO: Add your command handler code here
}

void CObjectPropertiesView::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
    // TODO: Add your command update UI handler code here
}

void CObjectPropertiesView::InitPropList()
{
    SetPropListFont();

    m_wndPropList.EnableHeaderCtrl(FALSE);
    m_wndPropList.EnableDescriptionArea();
    m_wndPropList.SetVSDotNetLook();
    m_wndPropList.MarkModifiedProperties();
    m_wndPropList.SetAlphabeticMode(FALSE);

    /*
    CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

    pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

    CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
    pProp->AddOption(_T("None"));
    pProp->AddOption(_T("Thin"));
    pProp->AddOption(_T("Resizable"));
    pProp->AddOption(_T("Dialog Frame"));
    pProp->AllowEdit(FALSE);

    pGroup1->AddSubItem(pProp);
    pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

    m_wndPropList.AddProperty(pGroup1);

    CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

    pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
    pProp->EnableSpinControl(TRUE, 50, 300);
    pSize->AddSubItem(pProp);

    pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
    pProp->EnableSpinControl(TRUE, 50, 200);
    pSize->AddSubItem(pProp);

    m_wndPropList.AddProperty(pSize);

    CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

    LOGFONT lf;
    CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
    font->GetLogFont(&lf);

    lstrcpy(lf.lfFaceName, _T("Arial"));

    pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
    pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

    m_wndPropList.AddProperty(pGroup2);

    CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
    pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
    pProp->Enable(FALSE);
    pGroup3->AddSubItem(pProp);

    CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
    pColorProp->EnableOtherButton(_T("Other..."));
    pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
    pGroup3->AddSubItem(pColorProp);

    static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
    pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

    pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

    m_wndPropList.AddProperty(pGroup3);

    CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

    CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
    pGroup4->AddSubItem(pGroup41);

    CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
    pGroup41->AddSubItem(pGroup411);

    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

    pGroup4->Expand(FALSE);
    m_wndPropList.AddProperty(pGroup4);
    */
}

void CObjectPropertiesView::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    m_wndPropList.SetFocus();
}

void CObjectPropertiesView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CWnd::OnSettingChange(uFlags, lpszSection);
    SetPropListFont();
}

void CObjectPropertiesView::SetPropListFont()
{
    ::DeleteObject(m_fntPropList.Detach());

    LOGFONT lf;
    afxGlobalData.fontRegular.GetLogFont(&lf);

    NONCLIENTMETRICS info;
    info.cbSize = sizeof(info);

    afxGlobalData.GetNonClientMetrics(info);

    lf.lfHeight = info.lfMenuFont.lfHeight;
    lf.lfWeight = info.lfMenuFont.lfWeight;
    lf.lfItalic = info.lfMenuFont.lfItalic;

    m_fntPropList.CreateFontIndirect(&lf);

    m_wndPropList.SetFont(&m_fntPropList);
    m_wndObjectCombo.SetFont(&m_fntPropList);
}

void CObjectPropertiesView::SetDocumentProperties(IContentStudioProperties* pProperties)
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

HRESULT CObjectPropertiesView::RefreshPropertiesView(IContentStudioProperties* pProperties)
{
    if (pProperties == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    m_wndPropList.RemoveAll();

    size_t numProps = 0;
    hr = pProperties->GetTotal(numProps);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < numProps; i++)
        {
            variant_t propValue;
            LPCSTR id = nullptr;
            hr = pProperties->GetIdByIndex(i, &id);
            if (SUCCEEDED(hr) && id != nullptr)
            {
                hr = pProperties->Get(id, propValue);
            }
            else
            {
                hr = E_UNEXPECTED;
            }

            if (SUCCEEDED(hr))
            {
                // convert 64-bit values to strings because the edit control
                // cannot handle displaying/editing these values.
                if (propValue.vt == VT_UI8)
                {
                    propValue.ChangeType(VT_BSTR);
                }

                std::wstring propNameW = stde::to_wstring(id);
                std::string propName = id;
                CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(propNameW.c_str(), COleVariant(propValue));
                
                // Ensure that some properties remain readonly
                if (propName == IdProperty ||
                    propName == FullPathProperty ||
                    propName == IsFolderProperty)
                {
                    pProp->AllowEdit(0);
                    pProp->Enable(0);
                }
                m_wndPropList.AddProperty(pProp);
            }
        }
    }

    return hr;
}

HRESULT CObjectPropertiesView::OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps)
{
    bool selectionChanged = false;
    for (size_t i = 0; i < numProps; i++)
    {
        std::string prop = changedProperties[i];
        if (prop == SelectedObjectProperty)
        {
            selectionChanged = true;
        }
    }

    // update properties
    if (selectionChanged)
    {
        // TODO: update properties to reflect currently select object's properties
        stde::com_ptr<IContentStudioProperties> spNewObject;
        if (SUCCEEDED(pProperties->GetInterface(SelectedObjectProperty, (IUnknown**)&spNewObject)))
        {
            RefreshPropertiesView(spNewObject);
        }
    }

    return S_OK;
}
