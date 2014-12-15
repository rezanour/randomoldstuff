// NameWorldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContentStudio.h"
#include "NameWorldDlg.h"
#include "afxdialogex.h"


// NameWorldDlg dialog

IMPLEMENT_DYNAMIC(NameWorldDlg, CDialogEx)

NameWorldDlg::NameWorldDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(NameWorldDlg::IDD, pParent)
    , _worldName(_T("untitled"))
{

}

NameWorldDlg::NameWorldDlg(_In_ std::wstring worldName)
    : CDialogEx(NameWorldDlg::IDD, nullptr)
    , _worldName(worldName.c_str())
{

}

NameWorldDlg::~NameWorldDlg()
{
}

std::wstring NameWorldDlg::GetName()
{
    std::wstring name;
    name = _worldName.GetBuffer();
    return name;
}

void NameWorldDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_WORLD_NAME_EDIT, _worldName);
    DDV_MaxChars(pDX, _worldName, 32);
}

BEGIN_MESSAGE_MAP(NameWorldDlg, CDialogEx)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

