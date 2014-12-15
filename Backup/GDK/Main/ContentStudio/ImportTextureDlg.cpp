// ImportTextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContentStudio.h"
#include "ImportTextureDlg.h"
#include "afxdialogex.h"

struct TEXTURE_RESOURCE_FORMAT_STRING
{
    TEXTURE_RESOURCE_FORMAT format;
    LPCWSTR string;
};

TEXTURE_RESOURCE_FORMAT_STRING g_formatToStringTable[] =
{
    {TEXTURE_RESOURCE_FORMAT_UNKNOWN                     ,L"TEXTURE_RESOURCE_FORMAT_UNKNOWN"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32A32_TYPELESS       ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32A32_TYPELESS "},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32A32_FLOAT          ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32A32_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32A32_UINT           ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32A32_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32A32_SINT           ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32A32_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32_TYPELESS          ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32_FLOAT             ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32_UINT              ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32B32_SINT              ,L"TEXTURE_RESOURCE_FORMAT_R32G32B32_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_TYPELESS       ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_FLOAT          ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UNORM          ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UINT           ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SNORM          ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SINT           ,L"TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32_TYPELESS             ,L"TEXTURE_RESOURCE_FORMAT_R32G32_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R32G32_FLOAT                ,L"TEXTURE_RESOURCE_FORMAT_R32G32_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32_UINT                 ,L"TEXTURE_RESOURCE_FORMAT_R32G32_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G32_SINT                 ,L"TEXTURE_RESOURCE_FORMAT_R32G32_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R32G8X24_TYPELESS           ,L"TEXTURE_RESOURCE_FORMAT_R32G8X24_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_D32_FLOAT_S8X24_UINT        ,L"TEXTURE_RESOURCE_FORMAT_D32_FLOAT_S8X24_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R32_FLOAT_X8X24_TYPELESS    ,L"TEXTURE_RESOURCE_FORMAT_R32_FLOAT_X8X24_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_X32_TYPELESS_G8X24_UINT     ,L"TEXTURE_RESOURCE_FORMAT_X32_TYPELESS_G8X24_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R10G10B10A2_TYPELESS        ,L"TEXTURE_RESOURCE_FORMAT_R10G10B10A2_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UNORM           ,L"TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UINT            ,L"TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R11G11B10_FLOAT             ,L"TEXTURE_RESOURCE_FORMAT_R11G11B10_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_TYPELESS           ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM              ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM_SRGB         ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UINT               ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SNORM              ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SINT               ,L"TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_TYPELESS             ,L"TEXTURE_RESOURCE_FORMAT_R16G16_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_FLOAT                ,L"TEXTURE_RESOURCE_FORMAT_R16G16_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_UNORM                ,L"TEXTURE_RESOURCE_FORMAT_R16G16_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_UINT                 ,L"TEXTURE_RESOURCE_FORMAT_R16G16_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_SNORM                ,L"TEXTURE_RESOURCE_FORMAT_R16G16_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16G16_SINT                 ,L"TEXTURE_RESOURCE_FORMAT_R16G16_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R32_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_R32_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_D32_FLOAT                   ,L"TEXTURE_RESOURCE_FORMAT_D32_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R32_FLOAT                   ,L"TEXTURE_RESOURCE_FORMAT_R32_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_R32_UINT                    ,L"TEXTURE_RESOURCE_FORMAT_R32_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R32_SINT                    ,L"TEXTURE_RESOURCE_FORMAT_R32_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R24G8_TYPELESS              ,L"TEXTURE_RESOURCE_FORMAT_R24G8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_D24_UNORM_S8_UINT           ,L"TEXTURE_RESOURCE_FORMAT_D24_UNORM_S8_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R24_UNORM_X8_TYPELESS       ,L"TEXTURE_RESOURCE_FORMAT_R24_UNORM_X8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_X24_TYPELESS_G8_UINT        ,L"TEXTURE_RESOURCE_FORMAT_X24_TYPELESS_G8_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_TYPELESS               ,L"TEXTURE_RESOURCE_FORMAT_R8G8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_UNORM                  ,L"TEXTURE_RESOURCE_FORMAT_R8G8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_UINT                   ,L"TEXTURE_RESOURCE_FORMAT_R8G8_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_SNORM                  ,L"TEXTURE_RESOURCE_FORMAT_R8G8_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_SINT                   ,L"TEXTURE_RESOURCE_FORMAT_R8G8_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R16_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_R16_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R16_FLOAT                   ,L"TEXTURE_RESOURCE_FORMAT_R16_FLOAT"},
    {TEXTURE_RESOURCE_FORMAT_D16_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_D16_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_R16_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16_UINT                    ,L"TEXTURE_RESOURCE_FORMAT_R16_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R16_SNORM                   ,L"TEXTURE_RESOURCE_FORMAT_R16_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R16_SINT                    ,L"TEXTURE_RESOURCE_FORMAT_R16_SINT"},
    {TEXTURE_RESOURCE_FORMAT_R8_TYPELESS                 ,L"TEXTURE_RESOURCE_FORMAT_R8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_R8_UNORM                    ,L"TEXTURE_RESOURCE_FORMAT_R8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8_UINT                     ,L"TEXTURE_RESOURCE_FORMAT_R8_UINT"},
    {TEXTURE_RESOURCE_FORMAT_R8_SNORM                    ,L"TEXTURE_RESOURCE_FORMAT_R8_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_R8_SINT                     ,L"TEXTURE_RESOURCE_FORMAT_R8_SINT"},
    {TEXTURE_RESOURCE_FORMAT_A8_UNORM                    ,L"TEXTURE_RESOURCE_FORMAT_A8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R1_UNORM                    ,L"TEXTURE_RESOURCE_FORMAT_R1_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R9G9B9E5_SHAREDEXP          ,L"TEXTURE_RESOURCE_FORMAT_R9G9B9E5_SHAREDEXP"},
    {TEXTURE_RESOURCE_FORMAT_R8G8_B8G8_UNORM             ,L"TEXTURE_RESOURCE_FORMAT_R8G8_B8G8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_G8R8_G8B8_UNORM             ,L"TEXTURE_RESOURCE_FORMAT_G8R8_G8B8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC1_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC1_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC1_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC1_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC1_UNORM_SRGB              ,L"TEXTURE_RESOURCE_FORMAT_BC1_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_BC2_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC2_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC2_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC2_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC2_UNORM_SRGB              ,L"TEXTURE_RESOURCE_FORMAT_BC2_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_BC3_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC3_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC3_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC3_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC3_UNORM_SRGB              ,L"TEXTURE_RESOURCE_FORMAT_BC3_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_BC4_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC4_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC4_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC4_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC4_SNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC4_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC5_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC5_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC5_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC5_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC5_SNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC5_SNORM"},
    {TEXTURE_RESOURCE_FORMAT_B5G6R5_UNORM                ,L"TEXTURE_RESOURCE_FORMAT_B5G6R5_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_B5G5R5A1_UNORM              ,L"TEXTURE_RESOURCE_FORMAT_B5G5R5A1_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM              ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM              ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  ,L"TEXTURE_RESOURCE_FORMAT_R10G10B10_XR_BIAS_A2_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8A8_TYPELESS           ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8A8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM_SRGB         ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8X8_TYPELESS           ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8X8_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM_SRGB         ,L"TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_BC6H_TYPELESS               ,L"TEXTURE_RESOURCE_FORMAT_BC6H_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC6H_UF16                   ,L"TEXTURE_RESOURCE_FORMAT_BC6H_UF16"},
    {TEXTURE_RESOURCE_FORMAT_BC6H_SF16                   ,L"TEXTURE_RESOURCE_FORMAT_BC6H_SF16"},
    {TEXTURE_RESOURCE_FORMAT_BC7_TYPELESS                ,L"TEXTURE_RESOURCE_FORMAT_BC7_TYPELESS"},
    {TEXTURE_RESOURCE_FORMAT_BC7_UNORM                   ,L"TEXTURE_RESOURCE_FORMAT_BC7_UNORM"},
    {TEXTURE_RESOURCE_FORMAT_BC7_UNORM_SRGB              ,L"TEXTURE_RESOURCE_FORMAT_BC7_UNORM_SRGB"},
    {TEXTURE_RESOURCE_FORMAT_FORCE_UINT                  ,L"TEXTURE_RESOURCE_FORMAT_FORCE_UINT"},
};

struct TEXTURE_RESOURCE_DIMENSION_STRING
{
    TEXTURE_RESOURCE_DIMENSION dimension;
    LPCWSTR string;
};

TEXTURE_RESOURCE_DIMENSION_STRING g_dimensionToStringTable[] =
{
    {TEXTURE_RESOURCE_DIMENSION_UNKNOWN,   L"TEXTURE_RESOURCE_DIMENSION_UNKNOWN"},
    {TEXTURE_RESOURCE_DIMENSION_TEXTURE1D, L"TEXTURE_RESOURCE_DIMENSION_TEXTURE1D"},
    {TEXTURE_RESOURCE_DIMENSION_TEXTURE2D, L"TEXTURE_RESOURCE_DIMENSION_TEXTURE2D"},
    {TEXTURE_RESOURCE_DIMENSION_TEXTURE3D, L"TEXTURE_RESOURCE_DIMENSION_TEXTURE3D"},
};

// CImportTextureDlg dialog

IMPLEMENT_DYNAMIC(CImportTextureDlg, CDialog)

CImportTextureDlg::CImportTextureDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CImportTextureDlg::IDD, pParent)
{

}

CImportTextureDlg::~CImportTextureDlg()
{
}

HRESULT CImportTextureDlg::Initialize(IDirectXTextureResource* pResource)
{
    if (!pResource)
    {
        return E_INVALIDARG;
    }

    _textureResource = pResource;

    return S_OK;
}

void CImportTextureDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TEXTURE_IMPORT_PROPERTYGRID, m_properties);
    DDX_Control(pDX, IDC_PICTURE_CONTROL_STATIC, m_picture);
}


BEGIN_MESSAGE_MAP(CImportTextureDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CImportTextureDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CImportTextureDlg message handlers


void CImportTextureDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    CDialog::OnOK();
}

void CImportTextureDlg::InitializePropertiesControl()
{
    HRESULT hr = S_OK;
    TEXTURE_RESOURCE_METADATA trm = {0};
    hr = _textureResource->GetMetadata(&trm);

    m_properties.SetVSDotNetLook(true);
    m_properties.SetGroupNameFullWidth(true);
    SetCMFCPropertyGridCtrlWidth(m_properties, 100);

    // Add import location property
    CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Import"));
    CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Location"), _T("Location 1"), _T("Specifies the location where the resource should be imported"));
    pProp->AddOption(_T("Location 1"));
    pProp->AddOption(_T("Location 2"));
    pProp->AddOption(_T("Location 3"));
    pProp->AddOption(_T("Location 4"));
    pProp->AllowEdit(FALSE);
    
    pGroup1->AddSubItem(pProp);

    m_properties.AddProperty(pGroup1);

    // Add texture resource size property
    CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Metadata"));
    CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Size"), 0, TRUE);
    pSize->SetDescription(L"Specifies the width and height of the resource");

    CMFCPropertyGridProperty* pProp3 = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) (long)trm.width, _T("Specifies the resource's width"));
    pSize->AddSubItem(pProp3);

    CMFCPropertyGridProperty* pProp2 = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) (long)trm.height, _T("Specifies the resource's height"));
    pSize->AddSubItem(pProp2);

    pGroup2->AddSubItem(pSize);

    // Add depth resource property
    CMFCPropertyGridProperty* pDepth = new CMFCPropertyGridProperty(_T("Depth"), (_variant_t) (long)trm.depth, _T("Specifies the depth of the resource. This should be 1 for 1D or 2D resources"));
    pGroup2->AddSubItem(pDepth);

    // Add array size
    CMFCPropertyGridProperty* pArraySize = new CMFCPropertyGridProperty(_T("Images"), (_variant_t) (long)trm.arraySize, _T("Specifies the number of embedded images. For cubemaps, this should be a multiple of 6"));
    pGroup2->AddSubItem(pArraySize);

    // Add mips
    CMFCPropertyGridProperty* pMips = new CMFCPropertyGridProperty(_T("Mips"), (_variant_t) (long)trm.mipLevels, _T("Specifies the number of mipmaps"));
    pGroup2->AddSubItem(pMips);

    // Add format
    CMFCPropertyGridProperty* pFormat = new CMFCPropertyGridProperty(_T("Format"), ResourceFormatToString(trm.format), _T("Specifies the format"));
    for(size_t i = 0; i < ARRAYSIZE(g_formatToStringTable); i++)
    {
        pFormat->AddOption(g_formatToStringTable[i].string);
    }
    pFormat->AllowEdit(FALSE);

    pGroup2->AddSubItem(pFormat);

    // Add dimension
    CMFCPropertyGridProperty* pDimension = new CMFCPropertyGridProperty(_T("Dimension"), ResourceDimensionToString(trm.dimension), _T("Specifies the dimension"));
    for(size_t i = 0; i < ARRAYSIZE(g_dimensionToStringTable); i++)
    {
        pDimension->AddOption(g_dimensionToStringTable[i].string);
    }
    pDimension->AllowEdit(FALSE);
    pGroup2->AddSubItem(pDimension);

    m_properties.AddProperty(pGroup2);
}

LPCWSTR CImportTextureDlg::ResourceFormatToString(TEXTURE_RESOURCE_FORMAT format)
{
    for(size_t i = 0; i < ARRAYSIZE(g_formatToStringTable); i++)
    {
        if (g_formatToStringTable[i].format == format)
        {
            return g_formatToStringTable[i].string;
        }
    }

    return L"UNKNOWN";
}

TEXTURE_RESOURCE_FORMAT CImportTextureDlg::ResourceFormatStringToResourceFormat(LPCWSTR string)
{
    for(size_t i = 0; i < ARRAYSIZE(g_formatToStringTable); i++)
    {
        std::wstring s = g_formatToStringTable[i].string;
        if (s == string)
        {
            return g_formatToStringTable[i].format;
        }
    }

    return TEXTURE_RESOURCE_FORMAT_UNKNOWN;
}

LPCWSTR CImportTextureDlg::ResourceDimensionToString(TEXTURE_RESOURCE_DIMENSION dimension)
{
    for(size_t i = 0; i < ARRAYSIZE(g_dimensionToStringTable); i++)
    {
        if (g_dimensionToStringTable[i].dimension == dimension)
        {
            return g_dimensionToStringTable[i].string;
        }
    }

    return L"UNKNOWN";
}

TEXTURE_RESOURCE_DIMENSION CImportTextureDlg::ResourceDimensionStringToResourceDimension(LPCWSTR string)
{
    for(size_t i = 0; i < ARRAYSIZE(g_dimensionToStringTable); i++)
    {
        std::wstring s = g_dimensionToStringTable[i].string;
        if (s == string)
        {
            return g_dimensionToStringTable[i].dimension;
        }
    }

    return TEXTURE_RESOURCE_DIMENSION_UNKNOWN;
}

BOOL CImportTextureDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Change default FONT to a smaller one
    //CFont* pcurrentFont = m_properties.GetFont();   
    //LOGFONT lf = {0};    
    //pcurrentFont->GetLogFont(&lf);
    //m_smallFont.CreatePointFont(60, lf.lfFaceName);
    //m_properties.SetFont(&m_smallFont);

    InitializePropertiesControl();

    HBITMAP hPreviewImage = nullptr;
    if (SUCCEEDED(_textureResource->CreateHBITMAP(&hPreviewImage)))
    {
        m_picture.SetBitmap(hPreviewImage);
        DeleteObject(hPreviewImage);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
