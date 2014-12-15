#include "stdafx.h"
#include "ContentStudioView.h"
#include "xmtypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CContentStudioView, CDirect3DView)

BEGIN_MESSAGE_MAP(CContentStudioView, CDirect3DView)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_VIEW_FRAMERATECOUNTER, &CContentStudioView::OnViewFrameratecounter)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FRAMERATECOUNTER, &CContentStudioView::OnUpdateViewFrameratecounter)
    ON_WM_DROPFILES()
#ifdef ENABLE_UNITTESTS
    ON_COMMAND(ID_UNITTEST_MODELCONTENTSERIALIZATION, &CContentStudioView::OnUnittestModelcontentserialization)
    ON_COMMAND(ID_UNITTEST_MODELCONTENTRAWFILEOFFSETS, &CContentStudioView::OnUnittestModelcontentrawfileoffsets)
#endif
    ON_COMMAND(ID_FILE_NEWSPRITEFONT, &CContentStudioView::OnFileNewSpriteFont)
END_MESSAGE_MAP()

CContentStudioView::CContentStudioView()  :
    m_pMainFrame(NULL),
    m_pTxtHelper(NULL),
    m_pSamLinear(NULL),
    m_pRasterizerStateSolid(NULL),
    m_pRasterizerStateWireframe(NULL),
    m_pRasterizerStateWireframeNoCull(NULL),
    m_bShowFrameRateCounter(false),
    m_currentViewId(MODEL_CONTENT_VIEW_ID)
{
    IContentViewPtr pView;
    
    // Add null content view
    pView = std::make_shared<CNullContentView>();
    m_views.push_back(pView);

    // Add model content view
    pView = std::make_shared<CModelContentView>();
    m_views.push_back(pView);

    // Add texture content view
    pView = std::make_shared<CTextureContentView>();
    m_views.push_back(pView);

    // Add spritefont content view
    pView = std::make_shared<CSpriteFontContentView>();
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

    // Forward update notification to MainFrame, passing the CDocument
    pMainFrame->OnUpdate(NULL, ID_REFRESH_SOLUTION_EXPLORER, pDoc);
    //pMainFrame->OnUpdate(NULL, ID_REFRESH_OBJECTSVIEW, pDoc);
    //pMainFrame->OnUpdate(NULL, ID_REFRESH_TEXTURESVIEW, pDoc);
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

    pd3dImmediateContext->PSSetSamplers( 0, 1, &m_pSamLinear );

    CContentStudioDoc* pDoc = GetDocument();
    if (pDoc != NULL)
    {
        if (pDoc->m_bEnableWireFrame || pDoc->m_bEnableWireFrameNoCull)
        {
            pd3dImmediateContext->RSSetState(pDoc->m_bEnableWireFrame ? m_pRasterizerStateWireframe : m_pRasterizerStateWireframeNoCull);
        }
        else
        {
            pd3dImmediateContext->RSSetState(m_pRasterizerStateSolid );
        }
    }

    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnDraw(m_graphicsDevice, m_deviceContext, fTime, fElapsedTime);
    }

    if (m_pTxtHelper != NULL)
    {
        m_pTxtHelper->Begin();
        m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        m_pTxtHelper->SetInsertionPos( 5, 5 );

        if (m_bShowFrameRateCounter)
        {
            m_pTxtHelper->DrawDropShadowedTextLine(DXUTGetFrameStats( true ));
            //m_pTxtHelper->DrawDropShadowedTextLine(DXUTGetDeviceStats());
        }

        m_pTxtHelper->End();
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

    SET_DEBUG_VERBOSITY(DebugLevel::Info);

    hr = Graphics::CreateGraphicsDevice(pd3dDevice, DXUTGetDXGISwapChain(), &m_graphicsDevice);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Can't create device");

    hr = Graphics::CreateDeviceContext(m_graphicsDevice, m_graphicsDevice->GetImmediateContext(), &m_deviceContext);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Can't create context");

    // REMOVE
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnLoadContent(m_graphicsDevice, m_deviceContext);
    }

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    m_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &m_DialogResourceManager, 16 );

    // Create solid and wireframe rasterizer state objects
    D3D11_RASTERIZER_DESC RasterDesc;
    ZeroMemory( &RasterDesc, sizeof( D3D11_RASTERIZER_DESC ) );
    //RasterDesc.FrontCounterClockwise = TRUE;
    RasterDesc.FillMode = D3D11_FILL_SOLID;
    RasterDesc.CullMode = D3D11_CULL_BACK;
    RasterDesc.DepthClipEnable = TRUE;

    hr = pd3dDevice->CreateRasterizerState( &RasterDesc, &m_pRasterizerStateSolid );
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "CreateRasterizerState (solid)");
    DXUT_SetDebugName( m_pRasterizerStateSolid, "Solid" );

    RasterDesc.CullMode = D3D11_CULL_NONE;
    RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    hr = pd3dDevice->CreateRasterizerState( &RasterDesc, &m_pRasterizerStateWireframeNoCull );
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "CreateRasterizerState (wireframe no culling)");
    DXUT_SetDebugName( m_pRasterizerStateWireframeNoCull, "Wireframe No Culling" );

    RasterDesc.CullMode = D3D11_CULL_BACK;
    RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    hr = pd3dDevice->CreateRasterizerState( &RasterDesc, &m_pRasterizerStateWireframe );
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "CreateRasterizerState (wireframe backface culling)");
    DXUT_SetDebugName( m_pRasterizerStateWireframe, "Wireframe backface culling" );

    // Create state objects
    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory( &samDesc, sizeof(samDesc) );
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = pd3dDevice->CreateSamplerState( &samDesc, &m_pSamLinear );
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "CreateSamplerState");
    DXUT_SetDebugName( m_pSamLinear, "Linear" );

Exit:

    return hr;
}

void CContentStudioView::OnD3D11DestroyDevice()
{
    __super::OnD3D11DestroyDevice();

    CContentStudioDoc* pDoc = GetDocument();
    if (pDoc != NULL)
    {
        pDoc->Destroy();
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

    m_graphicsDevice.reset();
    m_deviceContext.reset();

    SAFE_DELETE(m_pTxtHelper);
    SAFE_RELEASE( m_pSamLinear );
    SAFE_RELEASE( m_pRasterizerStateSolid );
    SAFE_RELEASE( m_pRasterizerStateWireframe );
    SAFE_RELEASE( m_pRasterizerStateWireframeNoCull );
}

LRESULT CContentStudioView::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    if (m_currentViewId < m_views.size())
    {
        m_views[m_currentViewId]->OnMsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
    }
    return __super::MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
}

void CContentStudioView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    __super::OnUpdate(pSender, lHint, pHint);

    // Get document
    CContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Determine if a view needs to be switched and/or updated

    if (pDoc->m_bContentUpdated)
    {
        UINT oldViewId = m_currentViewId;
        switch(pDoc->m_currentlySelectedContentType)
        {
        case spritefontContent:
            m_currentViewId = SPRITEFONT_CONTENT_VIEW_ID;
            break;
        case textureContent:
            m_currentViewId = TEXTURE_CONTENT_VIEW_ID;
            break;
        case modelContent:
            m_currentViewId = MODEL_CONTENT_VIEW_ID;
            break;
        default:
            m_currentViewId = NULL_VIEW_ID;
            break;
        }

        // unload any previously loaded content
        m_views[oldViewId]->OnUnloadContent();

        // load new content
        if (m_currentViewId < m_views.size())
        {
            m_views[m_currentViewId]->SetDocument(pDoc);
            m_views[m_currentViewId]->OnLoadContent(m_graphicsDevice, m_deviceContext);
            m_views[m_currentViewId]->OnResize(DXUTGetDXGIBackBufferSurfaceDesc()->Width, DXUTGetDXGIBackBufferSurfaceDesc()->Height);
            m_views[m_currentViewId]->OnRefresh(pDoc);
        }
    }

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

//void CContentStudioView::OnDraw(CDC* /*pDC*/)
//{
//    CContentStudioDoc* pDoc = GetDocument();
//    ASSERT_VALID(pDoc);
//    if (!pDoc)
//        return;
//}

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
    HRESULT hr = S_OK;
    WCHAR szFilePath[MAX_PATH] = {0};
    UINT numFiles = 0;

    numFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, szFilePath, ARRAYSIZE(szFilePath));
    for (UINT i = 0; i < numFiles; i++)
    {
        ZeroMemory(szFilePath, sizeof(szFilePath));
        if (DragQueryFile(hDropInfo, i, szFilePath, ARRAYSIZE(szFilePath)) != 0)
        {
            CContentPluginAccess* pPlugin = CContentPluginManager::CreatePluginForFile(szFilePath, FALSE);
            if (pPlugin != NULL)
            {
                HRESULT hr = S_OK;

                CContentPluginServices* pServices = new CContentPluginServices();
                hr = pPlugin->LoadContent(szFilePath, pServices);
                if (SUCCEEDED(hr))
                {
                    DEBUG_PRINT("Successfully loaded '%ws' as content", szFilePath);

                    // Get container
                    IContentContainerPtr pContainer = pServices->GetContentContainer();
                    CContentStudioDoc* pDoc = GetDocument();
                    pDoc->AddContent(szFilePath, pContainer);
                }
                else
                {
                    DEBUG_PRINT("Failed to load '%ws' as content", szFilePath);
                }
                SAFE_DELETE(pServices);
            }
            else
            {
                DEBUG_PRINT("'%ws' is not supported content", szFilePath);
            }
            SAFE_DELETE(pPlugin);
        }
    }
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

#ifdef ENABLE_UNITTESTS

void CContentStudioView::OnUnittestModelcontentserialization()
{
    HRESULT hr = S_OK;
    LPCWSTR modelFile = L"testmodel.mc";
    IContentContainerPtr pContentContainer;
    IModelContentPtr pLoadedModelContent;
    IModelContentPtr pSavedModelContent;
    CContentPluginServices* pServices = new CContentPluginServices();
    
    // Test Saving of ModelContent to file
    CContentPluginAccess* pPlugin = CContentPluginManager::CreatePluginForFile(modelFile, FALSE);
    if (pPlugin == NULL)
    {
        DEBUG_PRINT("UNITTEST: Failed to find a plugin that generates model content");
        goto Exit;
    }

    hr = pPlugin->LoadContentEx(modelFile, pServices);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "UNITTEST: Plugin failed to load model content file '%ws', hr = 0x%lx ", modelFile, hr);
        goto Exit;
    }

    pContentContainer = pServices->GetContentContainer();
    if (pContentContainer == NULL)
    {
        DEBUG_PRINT("UNITTEST: Failed to get a content container from plugin services");
        goto Exit;
    }

    pSavedModelContent = pContentContainer->GetModelContent(0);
    if (pSavedModelContent == NULL)
    {
        DEBUG_PRINT("UNITTEST: Failed to get first model content from content container");
        goto Exit;
    }

    hr = pSavedModelContent->SaveToFile(modelFile);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "UNITTEST: Failed to save model content");

    // Test Loading of ModelContent from file
    pLoadedModelContent = CModelContent::FromFile(modelFile);
    if (pLoadedModelContent == NULL)
    {
        DEBUG_PRINT( "UNITTEST: Failed to load model content file '%ws', hr = 0x%lx ", modelFile, hr);
        goto Exit;
    }

Exit:

    if (SUCCEEDED(hr))
    {
        DEBUG_PRINT("UNITTEST: Successfully saved and loaded ModelContent");
    }

    pSavedModelContent.reset();
    pLoadedModelContent.reset();
    pContentContainer.reset();
    SAFE_DELETE(pPlugin);
    SAFE_DELETE(pServices);
}

void CContentStudioView::OnUnittestModelcontentrawfileoffsets()
{
    HRESULT hr = S_OK;
    LPCWSTR modelFile = L"testmodel.mc";
    IStream* pStream = NULL;
    DWORD cbRead = 0;

    MODELCONTENTFILEHEADER fileHeader = {0};
    hr = FileStream::OpenFile(modelFile, &pStream, false);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "UNITTEST: Failed to open model content file '%ws' for RAW access, hr = 0x%lx ", modelFile, hr);
        goto Exit;
    }

    hr = pStream->Read(&fileHeader, sizeof(fileHeader), &cbRead);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "UNITTEST: Failed to read file header, hr = 0x%lx ", hr);
        goto Exit;
    }

    if (fileHeader.numIndexBuffers != 2)
    {
        hr = E_FAIL;
        DEBUG_PRINT( "UNITTEST: Model content file '%ws' contains invalid index buffer count (found %d, expected %d)", modelFile, fileHeader.numIndexBuffers, 2);
        goto Exit;
    }

    if (fileHeader.numVertexBuffers != 2)
    {
        hr = E_FAIL;
        DEBUG_PRINT( "UNITTEST: Model content file '%ws' contains invalid vertex buffer count (found %d, expected %d)", modelFile, fileHeader.numVertexBuffers, 2);
        goto Exit;
    }

    if (fileHeader.numMaterials != 2)
    {
        hr = E_FAIL;
        DEBUG_PRINT( "UNITTEST: Model content file '%ws' contains invalid materials count (found %d, expected %d)", modelFile, fileHeader.numMaterials, 2);
        goto Exit;
    }

    if (fileHeader.numMeshes != 2)
    {
        hr = E_FAIL;
        DEBUG_PRINT( "UNITTEST: Model content file '%ws' contains invalid meshes count (found %d, expected %d)", modelFile, fileHeader.numMeshes, 2);
        goto Exit;
    }

    // Seek to materials and output them
    hr = FileStream::SeekStream(pStream, STREAM_SEEK_SET, fileHeader.offsetMaterials);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to materials section");

    for (UINT i = 0; i < fileHeader.numMaterials; i++)
    {
        MODELCONTENTMATERIALHEADER materialHeader = {0};
        hr = pStream->Read(&materialHeader, sizeof(materialHeader), &cbRead);
        if (FAILED(hr))
        {
            DEBUG_PRINT( "UNITTEST: Failed to read material header, hr = 0x%lx ", hr);
            goto Exit;
        }

        DEBUG_PRINT("Material Id = %d", i);
        DEBUG_PRINT("Diffuse Texture = %s", materialHeader.diffuse);
        DEBUG_PRINT("Specular Texture = %s", materialHeader.specular);
        DEBUG_PRINT("Normal Texture = %s", materialHeader.normal);
        DEBUG_PRINT("--------------------------");
    }

    // Seek to vertices and output them
    hr = FileStream::SeekStream(pStream, STREAM_SEEK_SET, fileHeader.offsetVertexBuffers);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to vertices section");

    for (UINT i = 0; i < fileHeader.numVertexBuffers; i++)
    {
        MODELCONTENTBUFFERHEADER bufferHeader = {0};
        hr = pStream->Read(&bufferHeader, sizeof(bufferHeader), &cbRead);
        if (FAILED(hr))
        {
            DEBUG_PRINT( "UNITTEST: Failed to read vertices buffer header, hr = 0x%lx ", hr);
            goto Exit;
        }

        DEBUG_PRINT("Vertices Buffer Id = %d", i);
        DEBUG_PRINT("Number of Elements = %d", bufferHeader.numElements);
        DEBUG_PRINT("Stride = %d bytes", bufferHeader.stride);
        hr = FileStream::SeekStream(pStream, STREAM_SEEK_CUR, (bufferHeader.numElements * bufferHeader.stride));
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to vertices section");
        DEBUG_PRINT("--------------------------");
    }

    // Seek to indices and output them
    hr = FileStream::SeekStream(pStream, STREAM_SEEK_SET, fileHeader.offsetIndexBuffers);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to indices section");

    for (UINT i = 0; i < fileHeader.numIndexBuffers; i++)
    {
        MODELCONTENTBUFFERHEADER bufferHeader = {0};
        hr = pStream->Read(&bufferHeader, sizeof(bufferHeader), &cbRead);
        if (FAILED(hr))
        {
            DEBUG_PRINT( "UNITTEST: Failed to read indices buffer header, hr = 0x%lx ", hr);
            goto Exit;
        }

        DEBUG_PRINT("Indices Buffer Id = %d", i);
        DEBUG_PRINT("Number of Elements = %d", bufferHeader.numElements);
        DEBUG_PRINT("Stride = %d bytes", bufferHeader.stride);
        hr = FileStream::SeekStream(pStream, STREAM_SEEK_CUR, (bufferHeader.numElements * bufferHeader.stride));
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to indices section");
        DEBUG_PRINT("--------------------------");
    }

    // Seek to meshes and output them
    hr = FileStream::SeekStream(pStream, STREAM_SEEK_SET, fileHeader.offsetMeshes);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to meshes section");

    for (UINT i = 0; i < fileHeader.numMeshes; i++)
    {
        MODELCONTENTMESHHEADER meshHeader = {0};
        hr = pStream->Read(&meshHeader, sizeof(meshHeader), &cbRead);
        if (FAILED(hr))
        {
            DEBUG_PRINT( "UNITTEST: Failed to read meshes buffer header, hr = 0x%lx ", hr);
            goto Exit;
        }

        DEBUG_PRINT("Mesh Name = %s", meshHeader.name);
        DEBUG_PRINT("Mesh Index Buffer Id = %d", meshHeader.info.indexBufferId);
        DEBUG_PRINT("Mesh Vertex Buffer Id = %d", meshHeader.info.vertexBufferId);
        DEBUG_PRINT("Mesh Material Id = %d", meshHeader.info.materialId);
        DEBUG_PRINT("--------------------------");
    }

Exit:

    if SUCCEEDED(hr)
    {
        DEBUG_PRINT("UNITEST: Successfully validated file offsets in model content header");
    }

   SAFE_RELEASE(pStream);
}

#endif // ENABLE_UNITTESTS

void CContentStudioView::OnFileNewSpriteFont()
{
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
        ISpriteFontContentPtr spriteFontContent = std::make_shared<CSpriteFontContent>();
        if (spriteFontContent)
        {
            hr = spriteFontContent->LoadFromLOGFONT(&lf);
            if (SUCCEEDED(hr))
            {
                CContentStudioDoc* pDoc = GetDocument();
                pDoc->AddContent(L"spritefont.fc", spriteFontContent);
            }
        }
    }
}
