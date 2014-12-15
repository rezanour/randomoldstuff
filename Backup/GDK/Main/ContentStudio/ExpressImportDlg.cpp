// ExpressImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContentStudio.h"
#include "ExpressImportDlg.h"
#include "afxdialogex.h"


// CExpressImportDlg dialog

IMPLEMENT_DYNAMIC(CExpressImportDlg, CDialogEx)

CExpressImportDlg::CExpressImportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExpressImportDlg::IDD, pParent)
{

}

CExpressImportDlg::~CExpressImportDlg()
{
}

void CExpressImportDlg::Initialize(LPCWSTR repository, LPCWSTR name)
{
    _name = name;
    _repository = repository;
}

void CExpressImportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXPRESS_IMPORT_PROPERTYGRID, m_properties);
    DDX_Control(pDX, IDC_DONTSHOWME_CHECKBOX, m_DontAskMeAgain);
}


BEGIN_MESSAGE_MAP(CExpressImportDlg, CDialogEx)
END_MESSAGE_MAP()


// CExpressImportDlg message handlers


BOOL CExpressImportDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_properties.SetVSDotNetLook(true);
    m_properties.SetGroupNameFullWidth(true);
    SetCMFCPropertyGridCtrlWidth(m_properties, 100);

    _repositoryProperty = new CMFCPropertyGridFileProperty(_T("Repository"), _repository.c_str());
    _nameProperty = new CMFCPropertyGridProperty(_T("Name"), (_variant_t)_name.c_str(), _T("Specifies the name of the asset"));

    m_properties.AddProperty(_repositoryProperty);
    m_properties.AddProperty(_nameProperty);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CExpressImportDlg::OnOK()
{
    if (_repositoryProperty->IsModified())
    {
        // update repository public value
        _repository = _repositoryProperty->GetValue().bstrVal;
    }

    if (_nameProperty->IsModified())
    {
        // update name public value
        _name = _nameProperty->GetValue().bstrVal;
    }

    _dontShowAgain = (m_DontAskMeAgain.GetCheck() == 1);

    CDialogEx::OnOK();
}
