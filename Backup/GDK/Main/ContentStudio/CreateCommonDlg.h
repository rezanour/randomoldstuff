#pragma once
#include "afxpropertygridctrl.h"

class CCreateCommonDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CCreateCommonDlg)

public:
    CCreateCommonDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CCreateCommonDlg();
    HRESULT Initialize(LPCWSTR title, IContentStudioProperties* pProperties);

// Dialog Data
    enum { IDD = IDD_CREATE_COMMON_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private:
    void InitializePropertiesControl();
public:
    CContentStudioPropertyGridCtrl m_properties;
    stde::com_ptr<IContentStudioProperties> _properties;
    std::wstring _title;
};
