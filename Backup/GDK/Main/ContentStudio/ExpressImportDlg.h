#pragma once
#include "afxpropertygridctrl.h"
#include "afxwin.h"


// CExpressImportDlg dialog

class CExpressImportDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CExpressImportDlg)

public:
    CExpressImportDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CExpressImportDlg();
    void Initialize(LPCWSTR repository, LPCWSTR name);

    enum { IDD = IDD_EXPRESS_IMPORT_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CMFCPropertyGridCtrl m_properties;
    CButton m_DontAskMeAgain;
    std::wstring _name;
    std::wstring _repository;
    bool _dontShowAgain;

    CMFCPropertyGridFileProperty* _repositoryProperty;
    CMFCPropertyGridProperty* _nameProperty;
    virtual void OnOK();
};
