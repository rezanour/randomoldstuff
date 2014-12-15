#include "stdafx.h"
#include "ContentStudioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CContentStudioDocumentNotications;

IMPLEMENT_DYNCREATE(CContentStudioView, CDirect3DView)

BEGIN_MESSAGE_MAP(CContentStudioView, CDirect3DView)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_VIEW_FRAMERATECOUNTER, &CContentStudioView::OnViewFrameratecounter)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FRAMERATECOUNTER, &CContentStudioView::OnUpdateViewFrameratecounter)
    ON_WM_DROPFILES()
    //ON_COMMAND(ID_FILE_NEW_SPRITEFONT, &CContentStudioView::OnFileNewSpriteFont)
    ON_COMMAND(ID_FILE_IMPORT, &CContentStudioView::OnFileImport)
    //ON_COMMAND(ID_FILE_EXPORT, &CContentStudioView::OnFileExport)
    ON_COMMAND(ID_DEBUG_ENABLETESTVIEW, &CContentStudioView::OnDebugEnableTestView)
    ON_UPDATE_COMMAND_UI(ID_DEBUG_ENABLETESTVIEW, &CContentStudioView::OnUpdateDebugEnableTestView)
    ON_COMMAND(ID_TOOLS_SHOWEXPRESSIMPORTPROMPT, &CContentStudioView::OnToolsShowExpressImportPrompt)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_SHOWEXPRESSIMPORTPROMPT, &CContentStudioView::OnUpdateToolsShowExpressImportPrompt)
    ON_COMMAND(ID_TOOLS_RUNGAME, &CContentStudioView::OnToolsRunGame)
    ON_WM_KEYDOWN()
    ON_COMMAND(ID_DEBUG_CREATECUBEGEOMETRYRESOURCEFILE, &CContentStudioView::OnDebugCreateCubeGeometryResourceFile)
    ON_COMMAND(ID_DEBUG_CREATEPLANEGEOMETRYRESOURCEFILE, &CContentStudioView::OnDebugCreatePlaneGeometryResourceFile)
    ON_COMMAND(ID_FILE_NEWDUDEONAPLANEPROJECT, &CContentStudioView::OnFileNewDudeOnAPlaneProject)
    ON_COMMAND(ID_DEBUG_GENERATEWOLFENSTEIN3DE1M1PROJECTCONTENT, &CContentStudioView::OnDebugGenerateWolfenstein3DE1M1)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_RUNGAME, &CContentStudioView::OnUpdateToolsRunGame)
    ON_COMMAND(ID_INSERT_SCREEN, &CContentStudioView::OnInsertScreen)
END_MESSAGE_MAP()

CContentStudioView::CContentStudioView()  :
    m_pMainFrame(nullptr),
    m_bShowFrameRateCounter(false),
    m_currentViewId(CONTENT_3DVIEW_ID)
{
    IContentViewPtr pView;
    
    // Add null content view
    pView = std::make_shared<CNullContentView>();
    m_views.push_back(pView);

    // Add 3d content view
    pView = std::make_shared<CContentStudio3DView>();
    m_views.push_back(pView);
}

CContentStudioView::~CContentStudioView()
{
}

BOOL CContentStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CContentStudioView::OnInitialUpdate()
{
    // Allow files to be dropped onto the view
    DragAcceptFiles();

    m_pMainFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

    __super::OnInitialUpdate();

    // Get document
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    CContentStudioDocumentNotications* pNotifcations = new CContentStudioDocumentNotications(this);
    if (pNotifcations)
    {
        long long cke = 0;
        stde::com_ptr<IContentStudioProperties> spProps;
        pDoc->GetDocumentProperties(&spProps);
        spProps->RegisterPropertiesCallback(pNotifcations, cke);
        pNotifcations->Release();
    }

    // Set document on current view
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->SetDocument(pDoc);
        m_views[m_currentViewId]->OnRefresh(pDoc);
    }

    // Get mainframe to forward updates to owned docked controls
    CMainFrame* pMainFrame = GetMainFrame();
    ASSERT_VALID(pMainFrame);
    if (!pMainFrame)
        return;

    pMainFrame->OnViewsCreated(pDoc);
    pDoc->Refresh();
}

void CContentStudioView::OnFrameMove( double fTime, float fElapsedTime)
{
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnUpdate(fTime, fElapsedTime);
    }
}

void CContentStudioView::OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime)
{
    __super::OnD3D11FrameRender( pd3dDevice, pd3dImmediateContext, fTime, fElapsedTime);

    CContentStudioDoc* pDoc = GetDocument();
    if (pDoc != NULL)
    {
        
    }

    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnDraw(fTime, fElapsedTime);
    }   
}

HRESULT CContentStudioView::OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
    HRESULT hr = __super::OnD3D11ResizedSwapChain(pd3dDevice, pSwapChain, pBackBufferSurfaceDesc);

    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnResize(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
    }

    return hr;
}

HRESULT CContentStudioView::OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
    HRESULT hr = __super::OnD3D11CreateDevice(pd3dDevice, pBackBufferSurfaceDesc);

    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnLoadContent();
    }

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();

    return hr;
}

void CContentStudioView::OnD3D11DestroyDevice()
{
    __super::OnD3D11DestroyDevice();

    CContentStudioDoc* pDoc = GetDocument();
    if (pDoc != NULL)
    {
        pDoc->Clear();
    }

    for (UINT i = 0; i < m_views.size(); i++)
    {
        if (m_views[i])
        {
            m_views[i]->OnUnloadContent();
            m_views[i] = nullptr;
        }
    }

    m_views.clear();
}

LRESULT CContentStudioView::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnMsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
    }
    return __super::MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
}

void CContentStudioView::RefreshCurrentView()
{
    // Get document
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    m_views[m_currentViewId]->OnRefresh(pDoc);
}

void CContentStudioView::SwitchView(UINT viewId)
{
    // Get document
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    if (viewId != m_currentViewId)
    {
        UINT oldViewId = m_currentViewId;
        m_currentViewId = viewId;

        // unload any previously loaded content
        m_views[oldViewId]->OnUnloadContent();

        // load new content
        if (m_currentViewId < m_views.size())
        {
            m_views[m_currentViewId]->SetDocument(pDoc);
            m_views[m_currentViewId]->OnLoadContent();
            m_views[m_currentViewId]->OnResize(DXUTGetDXGIBackBufferSurfaceDesc()->Width, DXUTGetDXGIBackBufferSurfaceDesc()->Height);
            m_views[m_currentViewId]->OnRefresh(pDoc);
        }
    }
}

void CContentStudioView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    __super::OnUpdate(pSender, lHint, pHint);

    // Get document
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    m_views[m_currentViewId]->OnRefresh(pDoc);

    // Get mainframe to forward updates to owned docked controls
    CMainFrame* pMainFrame = GetMainFrame();
    if (pMainFrame != NULL)
    {
        ASSERT_VALID(pMainFrame);
        if (!pMainFrame)
            return;

        // Forward update notification to MainFrame, passing the CDocument
        pMainFrame->OnUpdate(pSender, lHint, pDoc);
    }
}

void CContentStudioView::OnDraw(CDC* pDC)
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Only perform 2D rendering if in 2D rendering mode
    if (GetCurrentRenderingMode() == RenderingMode::Render2D)
    {        
        CMemoryDC memDC(pDC);

        // Render a test grid
        CRect rect;
        GetClientRect (rect);
        memDC.FillSolidRect(rect, RGB(163,163,161));

        COLORREF clrTopLeft = 0x00000000;
        COLORREF clrBottomRight = 0x00000000;

        int spacing = 16;

        CPen newPen(PS_SOLID, 1, RGB(135,135,135));
        CPen *oldPen = memDC->SelectObject(&newPen);

        for (int row = 0; row < 64; row++)
        {
            memDC->MoveTo(0, row * spacing);
            memDC->LineTo(64 * spacing, row * spacing);
        }

        for (int col = 0; col < 64; col++)
        {
            memDC->MoveTo(col * spacing, 0);
            memDC->LineTo(col * spacing, 64 * spacing);
        }

        memDC->SelectObject(oldPen);
    }
    else
    {
        DXUTRender3DEnvironment();
    }
}

void CContentStudioView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CContentStudioView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{

}

#ifdef _DEBUG
void CContentStudioView::AssertValid() const
{
    CView::AssertValid();
}

void CContentStudioView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CContentStudioDoc* CContentStudioView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CContentStudioDoc)));
    return (CContentStudioDoc*)m_pDocument;
}

CMainFrame* CContentStudioView::GetMainFrame() const
{
    ASSERT(m_pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
    return (CMainFrame*)m_pMainFrame;
}
#endif //_DEBUG

RenderingMode CContentStudioView::GetCurrentRenderingMode()
{
    CContentStudioApp* pApp = (CContentStudioApp*)AfxGetApp();
    if (pApp)
    {
        return pApp->GetCurrentRenderingMode();
    }

    return RenderingMode::Render3D;
}

void CContentStudioView::ChangeRenderingMode(RenderingMode mode)
{
    CContentStudioApp* pApp = (CContentStudioApp*)AfxGetApp();
    if (pApp)
    {
        return pApp->ChangeRenderingMode(mode);
    }
}

void CContentStudioView::OnViewFrameratecounter()
{
    m_bShowFrameRateCounter = !m_bShowFrameRateCounter;
}

void CContentStudioView::OnUpdateViewFrameratecounter(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bShowFrameRateCounter);
}

void CContentStudioView::HandleDropFiles(HDROP hDropInfo)
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    HRESULT hr = S_OK;
    WCHAR szFilePath[MAX_PATH] = {0};
    UINT numFiles = 0;

    numFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, szFilePath, ARRAYSIZE(szFilePath));
    for (UINT i = 0; i < numFiles; i++)
    {
        ZeroMemory(szFilePath, sizeof(szFilePath));
        if (DragQueryFile(hDropInfo, i, szFilePath, ARRAYSIZE(szFilePath)) != 0)
        {
            HRESULT hr = S_OK;
            // szFilePath is file dropped
            CContentStudioDoc* pDoc = GetDocument();
            DEBUG_PRINT("'%ws' dropped onto view...", szFilePath);
            OnImportFile(szFilePath);
        }
    }

    // refresh after large import drop
    pDoc->Refresh();
}

void CContentStudioView::OnDropFiles(HDROP hDropInfo)
{
    HandleDropFiles(hDropInfo);
    CDirect3DView::OnDropFiles(hDropInfo);
}

void CContentStudioView::OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                           bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                           int xPos, int yPos)
{
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnMouse(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown, bSideButton1Down, bSideButton2Down, nMouseWheelDelta, xPos, yPos);
    }
}

void CContentStudioView::OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown)
{
    if (nChar == VK_ESCAPE || bAltDown)
    {
        // Restore previous modes
    }

    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnKeyboard(nChar, bKeyDown, bAltDown);
    }
}

void CContentStudioView::OnFileNewSpriteFont()
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    HRESULT hr = S_OK;
    BOOL bResult = FALSE;
    static LOGFONT lf;
    CHOOSEFONT chooseFont = {0};
    chooseFont.lStructSize = sizeof(chooseFont);
    chooseFont.hwndOwner = m_hWnd;
    chooseFont.lpLogFont = &lf;
    bResult = ChooseFont(&chooseFont);
    if (bResult)
    {
        stde::com_ptr<ISpriteFontResource> spResource;
        stde::com_ptr<IDirectXTextureResource> spTextureResource;
        hr = SpriteFontResourceServices::FromLOGFONT(&lf, true, &spResource);
        if (SUCCEEDED(hr))
        {
            hr = spResource->GetTextureResource((ITextureResource**)&spTextureResource);
            if (SUCCEEDED(hr))
            {
                CImportTextureDlg itd;
                if (SUCCEEDED(itd.Initialize(spTextureResource)))
                {
                    if (itd.DoModal() == IDOK)
                    {
                        stde::com_ptr<IContentStudioProperties> props;
                        pDoc->GetDocumentProperties(&props);
                        props->SetInterface("ImportTextureTest",itd.GetTextureResource());
                        pDoc->UpdateAllViews(nullptr);
                    }
                }
            }
        }        
    }
}

void CContentStudioView::OnFileImport()
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    std::wstring supportedFilesFilter = DataExtensionManager::GetSupportedFilesFilter();

    INT dialogResult = IDCANCEL;
    DWORD flags = 0;
    OPENFILENAME ofn = {0};
    CFileDialog importFileDialog(TRUE, NULL, NULL, flags, supportedFilesFilter.c_str());
    importFileDialog.GetOFN().lpstrTitle = L"Import";
    importFileDialog.GetOFN().nFilterIndex = ((CContentStudioApp*)AfxGetApp())->GetInt(L"ImportFileFilterIndex",1000);
    
    dialogResult = importFileDialog.DoModal();
    // Always persist last selected file filter type
    ((CContentStudioApp*)AfxGetApp())->WriteInt(L"ImportFileFilterIndex",importFileDialog.GetOFN().nFilterIndex);
    if (dialogResult == IDOK)
    {
        OnImportFile(importFileDialog.m_pOFN->lpstrFile);
        pDoc->Refresh();
    }
}

void CContentStudioView::OnImportFile(LPCWSTR filePath)
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    if (pDoc->GetPathName().IsEmpty())
    {
        //AfxMessageBox(L"Please create a new game project before performing an import operation", MB_ICONINFORMATION|MB_OK);
        //return;
        if (FAILED(pDoc->CreateDocument()))
        {
            return;
        }
    }

    HRESULT hr = S_OK;
    stde::com_ptr<IStream> fileStream;
    stde::com_ptr<IDirectXTextureResource> textureResource;
    if (SUCCEEDED(CoreServices::FileStream::Create(filePath, true, &fileStream)))
    {
        hr = TextureResourceServices::FromStream(fileStream, &textureResource);
        if (SUCCEEDED(hr))
        {
            bool importPrompt = (((CContentStudioApp*)AfxGetApp())->GetInt(L"SuppressExpressPrompt",0) == 0);
            bool proceedWithImport = true;

            stde::com_ptr<IContentStudioProperties> spDocumentProperties;
            pDoc->GetDocumentProperties(&spDocumentProperties);

            CExpressImportDlg importDlg;
            std::wstring assetName = FileFromFilePath(filePath, FALSE);
            std::wstring respositoryPath = ReadStringPropertyW(ContentRepositoryRootProperty, spDocumentProperties);

            if (importPrompt)
            {
                importDlg.Initialize(respositoryPath.c_str(), assetName.c_str());
                if (importDlg.DoModal() == IDOK)
                {
                    assetName = importDlg._name;
                    respositoryPath = importDlg._repository;
                    ((CContentStudioApp*)AfxGetApp())->WriteInt(L"SuppressExpressPrompt",importDlg._dontShowAgain ? 1 : 0);
                }
                else
                {
                    proceedWithImport = false;
                }
            }

            if (proceedWithImport)
            {
                // Save resource
                stde::com_ptr<IStream> resourceFileStream;
                std::wstring resourceFilePath;
                uint64 texId = 0;
                textureResource->GetId(texId);
                resourceFilePath = respositoryPath + stde::to_wstring<uint64>(texId) + L".textureresource";
                CoreServices::FileStream::Create(resourceFilePath, false, &resourceFileStream);
                textureResource->SetName(stde::to_string(assetName).c_str());
                textureResource->Save(resourceFileStream, TextureResourceSaveFormat::textureresource);
                resourceFileStream = nullptr;
            }

            // KEEP: Below is code that demonstrates ContentStudio's texture import
            //       dialog.  Eventually this will become an advanced experience
            //       in later milestones.
            /*
            CImportTextureDlg itd;
            if (SUCCEEDED(itd.Initialize(textureResource)))
            {
                if (itd.DoModal() == IDOK)
                {
                    stde::com_ptr<IContentStudioProperties> props;
                    pDoc->GetDocumentProperties(&props);
                    props->SetInterface("ImportTextureTest",itd.GetTextureResource());
                    pDoc->UpdateAllViews(nullptr);
                }
            }
            */
        }
        else
        {
            DEBUG_PRINT("Failed to import '%ws' into texture resource format, hr = 0x%lx", filePath, hr);
            AfxMessageBox(L"Only image files are supported for import", MB_ICONERROR|MB_OK);
        }
    }
}

void CContentStudioView::OnDebugEnableTestView()
{
    if (m_currentViewId == NULL_VIEW_ID)
    {
        SwitchView(CONTENT_3DVIEW_ID);
        ChangeRenderingMode(RenderingMode::Render3D);
    }
    else
    {
        SwitchView(NULL_VIEW_ID);
        ChangeRenderingMode(RenderingMode::Render2D);
        Invalidate();
    }
}

void CContentStudioView::OnUpdateDebugEnableTestView(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((m_currentViewId == CONTENT_3DVIEW_ID));
}

void CContentStudioView::OnFileExport()
{
    std::wstring supportedFilesFilter = DataExtensionManager::GetSupportedFilesFilter();

    INT dialogResult = IDCANCEL;
    DWORD flags = 0;
    OPENFILENAME ofn = {0};
    CFileDialog exportFileDialog(FALSE, NULL, NULL, flags, supportedFilesFilter.c_str());
    exportFileDialog.GetOFN().lpstrTitle = L"Export";

    exportFileDialog.GetOFN().nFilterIndex = ((CContentStudioApp*)AfxGetApp())->GetInt(L"ExportFileFilterIndex",1000);
    
    dialogResult = exportFileDialog.DoModal();
    // Always persist last selected file filter type
    ((CContentStudioApp*)AfxGetApp())->WriteInt(L"ExportFileFilterIndex",exportFileDialog.GetOFN().nFilterIndex);
    if (dialogResult == IDOK)
    {
        std::vector<std::wstring> extensions;
        std::wstring initFilter;
        std::wstring fileExtension;
        ConvertFilterWithEmbeddedNullsInitialFilterVersion(exportFileDialog.GetOFN().lpstrFilter, initFilter);
        GetExtensionsListFromFilter(initFilter.c_str() ,extensions);
        
        fileExtension = extensions[exportFileDialog.GetOFN().nFilterIndex - 1];
        if (!fileExtension.empty())
        {
            std::wstring newPath = FilePathwithNewExtension(exportFileDialog.m_pOFN->lpstrFile, fileExtension.c_str());
            OnExportFile(newPath.c_str());
        }
        else
        {
            OnExportFile(exportFileDialog.m_pOFN->lpstrFile);
        }
    }
}

HRESULT TextureResourceFormatFromFile(LPCWSTR filePath, TextureResourceSaveFormat& fmt)
{
    struct fileFormatInfo
    {
        TextureResourceSaveFormat type; 
        LPCWSTR                   extension;
    };

    fileFormatInfo supportedSaveExtensions[] = {
        {TextureResourceSaveFormat::bmp,   L".bmp"},
        {TextureResourceSaveFormat::png,   L".png"},
        {TextureResourceSaveFormat::jpg,   L".jpg"},
        {TextureResourceSaveFormat::dds,   L".dds"},
        {TextureResourceSaveFormat::tga,   L".tga"},
        {TextureResourceSaveFormat::textureresource,   L".textureresource"},
    };

    std::wstring strFilePath = filePath;
    for (UINT i = 0; i < ARRAYSIZE(supportedSaveExtensions); i++)
    {
        if (strFilePath.rfind(supportedSaveExtensions[i].extension) != std::wstring::npos)
        {
            fmt = supportedSaveExtensions[i].type;
            return S_OK;
        }
    }

    return E_NOTIMPL;
}

void CContentStudioView::OnExportFile(LPCWSTR filePath)
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    HRESULT hr = S_OK;
    TextureResourceSaveFormat textureSaveFormat = TextureResourceSaveFormat::textureresource;
    hr = TextureResourceFormatFromFile(filePath, textureSaveFormat);
    if (SUCCEEDED(hr))
    {
        stde::com_ptr<IStream> fileStream;
        stde::com_ptr<IDirectXTextureResource> textureResource;
        if (SUCCEEDED(CoreServices::FileStream::Create(filePath, false, &fileStream)))
        {
            if (SUCCEEDED(hr))
            {
                stde::com_ptr<IContentStudioProperties> props;
                pDoc->GetDocumentProperties(&props);

                if (SUCCEEDED(props->GetInterface("ImportTextureTest",(IUnknown**)&textureResource)))
                {
                    hr = textureResource->Save(fileStream, textureSaveFormat);
                }
            }
        }
    }

    if FAILED(hr)
    {
        DEBUG_PRINT("Failed to export '%ws' ..., hr = 0x%lx", filePath, hr);
    }
}

void CContentStudioView::OnToolsShowExpressImportPrompt()
{
    int suppressExpressPrompt = ((CContentStudioApp*)AfxGetApp())->GetInt(L"SuppressExpressPrompt",0);
    if (suppressExpressPrompt == 1)
    {
        suppressExpressPrompt = 0;
    }
    else
    {
        suppressExpressPrompt = 1;
    }

    ((CContentStudioApp*)AfxGetApp())->WriteInt(L"SuppressExpressPrompt",suppressExpressPrompt);
}


void CContentStudioView::OnUpdateToolsShowExpressImportPrompt(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(((CContentStudioApp*)AfxGetApp())->GetInt(L"SuppressExpressPrompt",0));
}


void CContentStudioView::OnToolsRunGame()
{
    std::wstring iniFilePath;
    std::wstring quotediniFilePath;
    if (GetGDKIniFilePath(iniFilePath))
    {
        quotediniFilePath = L"GDKGame.exe \"" + iniFilePath + L"\"";
         
        WCHAR contentStudioFullPath[MAX_PATH] = {0};
        GetModuleFileName(nullptr, contentStudioFullPath, ARRAYSIZE(contentStudioFullPath) - 1);
        std::wstring contentStudioDir = FilePathOnlyEx(contentStudioFullPath);

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

        DWORD creationFlags = 0;
        if (!CreateProcess(nullptr, (LPWSTR)quotediniFilePath.c_str(), nullptr, nullptr, false, creationFlags, nullptr, contentStudioDir.c_str(), &si, &pi))
        {
            DEBUG_PRINT("CreateProcess on GDKGame.exe failed, error code = %d", ::GetLastError());
        }
        else
        {
            // Close process and thread handles. 
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
        }
    }
}

void CContentStudioView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_F5)
    {
        CContentStudioDoc* pDoc = GetDocument();
        if (pDoc)
        {
            if (!pDoc->GetPathName().IsEmpty())
            {
                pDoc->Refresh();
            }
        }
    }

    CDirect3DView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CContentStudioView::OnDebugCreateCubeGeometryResourceFile()
{
    HRESULT hr = S_OK;
    CFileDialog saveAsDialog(FALSE, L"geometryresource", L"cube", 0, L"Geometry Resource Files (*.geometryresource)|*.geometryresource||");
    saveAsDialog.GetOFN().lpstrTitle = L"Create Cube Geometry Resource";
    if (saveAsDialog.DoModal() == IDOK)
    {
        stde::com_ptr<IGeometryResource> spResource;
        stde::com_ptr<IStream> spFileStream;
        hr = ResourceFactory::CreateCube("cube", &spResource);
        if (SUCCEEDED(hr))
        {
            hr = CoreServices::FileStream::Create(saveAsDialog.GetOFN().lpstrFile, false, &spFileStream);
        }

        if (SUCCEEDED(hr))
        {
            hr = spResource->Save(spFileStream);
            stde::com_ptr<IContentStudioProperties> spDocumentProperties;
            GetDocument()->GetDocumentProperties(&spDocumentProperties);
            spDocumentProperties->SetInterface("CubeResourceRenderTest", spResource);
            m_views[m_currentViewId]->OnRefresh(GetDocument());
        }
    }
}


void CContentStudioView::OnDebugCreatePlaneGeometryResourceFile()
{
    HRESULT hr = S_OK;
    CFileDialog saveAsDialog(FALSE, L"geometryresource", L"plane", 0, L"Geometry Resource Files (*.geometryresource)|*.geometryresource||");
    saveAsDialog.GetOFN().lpstrTitle = L"Create Plane Geometry Resource";
    if (saveAsDialog.DoModal() == IDOK)
    {
        stde::com_ptr<IGeometryResource> spResource;
        stde::com_ptr<IStream> spFileStream;

        hr = ResourceFactory::CreatePlane("plane", &spResource);
        if (SUCCEEDED(hr))
        {
            hr = CoreServices::FileStream::Create(saveAsDialog.GetOFN().lpstrFile, false, &spFileStream);
        }

        if (SUCCEEDED(hr))
        {
            hr = spResource->Save(spFileStream);
            stde::com_ptr<IContentStudioProperties> spDocumentProperties;
            GetDocument()->GetDocumentProperties(&spDocumentProperties);
            spDocumentProperties->SetInterface("CubeResourceRenderTest", spResource);
            m_views[m_currentViewId]->OnRefresh(GetDocument());
        }
    }
}


void CContentStudioView::OnFileNewDudeOnAPlaneProject()
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    pDoc->DebugCreateCubeOnPlaneProject(false);
    pDoc->Refresh();
}


void CContentStudioView::OnDebugGenerateWolfenstein3DE1M1()
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    pDoc->DebugCreateCubeOnPlaneProject(true);
    pDoc->Refresh();
}

bool CContentStudioView::GetGDKIniFilePath(std::wstring& iniFilePath)
{
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return false;

    stde::com_ptr<IContentStudioProperties> documentProperties;
    bool fileExists = false;

    if (SUCCEEDED(pDoc->GetDocumentProperties(&documentProperties)))
    {
        std::wstring fullPath = ReadStringPropertyW(ProjectRootProperty, documentProperties);
        iniFilePath = FilePathOnly(fullPath.c_str()) + L"GDKGame.ini";
        fileExists = CoreServices::FileStream::FileExists(iniFilePath);
    }

    return fileExists;
}

void CContentStudioView::OnUpdateToolsRunGame(CCmdUI *pCmdUI)
{
    std::wstring iniFilePath;
    bool enableMenuItem = GetGDKIniFilePath(iniFilePath);
    pCmdUI->Enable(enableMenuItem);
}

void CContentStudioView::OnInsertScreen()
{
    int PointSize = 14;
    LPCWSTR FaceName = L"Arial";

    static LOGFONT lf   = {0};
    lf.lfHeight         = -MulDiv(PointSize, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);//-19;
    lf.lfWeight         = 400;
    lf.lfQuality        = 1;
    lf.lfPitchAndFamily = 34;
    lf.lfOutPrecision   = 3;
    lf.lfClipPrecision  = 2;
    StringCchCopyW( lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), FaceName );

    stde::com_ptr<ISpriteFontResource> spResource;
    stde::com_ptr<IDirectXTextureResource> spTextureResource;
    HRESULT hr = SpriteFontResourceServices::FromLOGFONT(&lf, true, &spResource);
    if (SUCCEEDED(hr))
    {
        hr = spResource->GetTextureResource((ITextureResource**)&spTextureResource);
        if (SUCCEEDED(hr))
        {
            // Arial spritefont created
            RECT r[255];
            WCHAR n[64];
            CHAR c[255];
            size_t cGlyphs = 0;
            hr = spResource->GetNumGlyphs(&cGlyphs);
            hr = spResource->GetFontName(64, n);
            hr = spResource->GetGlyphRects(cGlyphs, r);
            hr = spResource->GetGlyphs(cGlyphs, c);
        }
    }

    /*
    CCreateCommonDlg dlg(this);
    stde::com_ptr<IContentStudioProperties> properties;
    HRESULT hr = S_OK;
    hr = ContentStudioProperties::Create(&properties);
    if (SUCCEEDED(hr))
    {
        float f = 3.14f;
        properties->SetFloat("Dookie Float", f);
        properties->SetString("Stinky", "Poop");

        hr = dlg.Initialize(L"Create Screen", properties);
        dlg.DoModal();
    }
    */
}
