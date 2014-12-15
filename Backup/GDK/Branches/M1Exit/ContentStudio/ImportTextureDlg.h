#pragma once
#include "afxpropertygridctrl.h"
#include "afxwin.h"


// CImportTextureDlg dialog

class CImportTextureDlg : public CDialog
{
    DECLARE_DYNAMIC(CImportTextureDlg)

public:
    CImportTextureDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CImportTextureDlg();
    HRESULT Initialize(IDirectXTextureResource* pResource);
    IDirectXTextureResource* GetTextureResource() {return _textureResource;}

private:
    void InitializePropertiesControl();
    stde::com_ptr<IDirectXTextureResource> _textureResource;

    LPCWSTR ResourceFormatToString(TEXTURE_RESOURCE_FORMAT format);
    TEXTURE_RESOURCE_FORMAT ResourceFormatStringToResourceFormat(LPCWSTR string);
    LPCWSTR ResourceDimensionToString(TEXTURE_RESOURCE_DIMENSION dimension);
    TEXTURE_RESOURCE_DIMENSION ResourceDimensionStringToResourceDimension(LPCWSTR string);

// Dialog Data
    enum { IDD = IDD_IMPORT_TEXTURE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CMFCPropertyGridCtrl m_properties;
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    CStatic m_picture;
    CFont m_smallFont;
};
