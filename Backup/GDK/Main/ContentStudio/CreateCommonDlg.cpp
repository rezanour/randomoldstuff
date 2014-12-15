#include "stdafx.h"
#include "ContentStudio.h"
#include "CreateCommonDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CCreateCommonDlg, CDialogEx)

CCreateCommonDlg::CCreateCommonDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CCreateCommonDlg::IDD, pParent)
{

}

CCreateCommonDlg::~CCreateCommonDlg()
{
}

void CCreateCommonDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CREATE_PROPERTY_GRID, m_properties);
}

BOOL CCreateCommonDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    InitializePropertiesControl();

    //m_properties.AddProperty(new CMFCPropertyGridProperty(_T("(Masked edit)"), _T("(123) 456-7890"), _T("Enter a phone number"), 0, _T(" ddd ddd dddd"), _T("(___) ___-____")));
    //m_properties.AddProperty(new CMFCPropertyGridProperty(_T("Position"), _T("0.0,0.0,0.0"), _T("Enter a position"), 0, _T("d.d d.d d.d"), _T("_._,_._,_._")));

    CMFCPropertyGridProperty* pPosition = new CMFCPropertyGridProperty(_T("PositionXYZ"), 0, TRUE);
    pPosition->SetDescription(L"Specifies the position of the object");
    pPosition->AddSubItem(new CMFCPropertyGridProperty(_T("X"), (_variant_t) (float)0, _T("Specifies the x coordinate")));
    pPosition->AddSubItem(new CMFCPropertyGridProperty(_T("Y"), (_variant_t) (float)0, _T("Specifies the y coordinate")));
    pPosition->AddSubItem(new CMFCPropertyGridProperty(_T("Z"), (_variant_t) (float)0, _T("Specifies the z coordinate")));
    m_properties.AddProperty(pPosition);

    CMFCPropertyGridProperty* pPositionXY = new CMFCPropertyGridProperty(_T("PositionXY"), 0, TRUE);
    pPositionXY->SetDescription(L"Specifies the position of the object");
    pPositionXY->AddSubItem(new CMFCPropertyGridProperty(_T("X"), (_variant_t) (float)0, _T("Specifies the x coordinate"),42));
    pPositionXY->AddSubItem(new CMFCPropertyGridProperty(_T("Y"), (_variant_t) (float)0, _T("Specifies the y coordinate")));
    m_properties.AddProperty(pPositionXY);

    for (int i = 0; i < m_properties.GetPropertyCount(); i++)
    {
        CMFCPropertyGridProperty* pProperty = m_properties.GetProperty(i);
        if (pProperty)
        {
            if (pProperty->IsGroup())
            {
                for (int s = 0; s < pProperty->GetSubItemsCount(); s++)
                {
                    CMFCPropertyGridProperty* pSubItemProperty = pProperty->GetSubItem(s);
                    if (pSubItemProperty)
                    {
                        DWORD_PTR dp = pSubItemProperty->GetData();
                        COleVariant v2 = pSubItemProperty->GetValue();
                        int dd = 0;
                    }
                }
            }
            else
            {
                COleVariant v = pProperty->GetValue();
                int cc = 0;
            }
        }
    }

    return TRUE;
}

HRESULT CCreateCommonDlg::Initialize(LPCWSTR title, IContentStudioProperties* pProperties)
{
    _title = title;
    _properties = pProperties;
    return S_OK;
}

void CCreateCommonDlg::InitializePropertiesControl()
{
    SetWindowText(_title.c_str());

    m_properties.SetVSDotNetLook(true);
    m_properties.SetGroupNameFullWidth(true);
    SetCMFCPropertyGridCtrlWidth(m_properties, 100);

    size_t numProps = 0;
    HRESULT hr = _properties->GetTotal(numProps);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < numProps; i++)
        {
            variant_t propValue;
            LPCSTR id = nullptr;
            hr = _properties->GetIdByIndex(i, &id);
            if (SUCCEEDED(hr) && id != nullptr)
            {
                hr = _properties->Get(id, propValue);
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
                m_properties.AddProperty(pProp);
            }
        }
    }
}

BEGIN_MESSAGE_MAP(CCreateCommonDlg, CDialogEx)
END_MESSAGE_MAP()

// CCreateCommonDlg message handlers
