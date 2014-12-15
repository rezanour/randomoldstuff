#pragma once


// NameWorldDlg dialog

class NameWorldDlg : public CDialogEx
{
    DECLARE_DYNAMIC(NameWorldDlg)

public:
    NameWorldDlg(_In_ CWnd* pParent = NULL);   // standard constructor
    NameWorldDlg(_In_ std::wstring worldName);
    virtual ~NameWorldDlg();

// Dialog Data
    enum { IDD = IDD_NAME_WORLD_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    std::wstring GetName();

public:
    CString _worldName;
};
