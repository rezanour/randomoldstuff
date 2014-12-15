#include "stdafx.h"
#include "ContentStudioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ContentStudioView, CView)

BEGIN_MESSAGE_MAP(ContentStudioView, CView)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_WM_NCDESTROY()
    ON_WM_INITMENUPOPUP()
    ON_COMMAND_RANGE(ID_GRAPHICS_, ID_GRAPHICS_ + MAX_RENDERERS, &ContentStudioView::OnGraphics)
    ON_UPDATE_COMMAND_UI_RANGE(ID_GRAPHICS_, ID_GRAPHICS_ + MAX_RENDERERS, &ContentStudioView::OnUpdateGraphics)
    ON_COMMAND_RANGE(ID_GAMES_, ID_GAMES_ + MAX_GAMES, &ContentStudioView::OnGames)
    ON_UPDATE_COMMAND_UI_RANGE(ID_GAMES_, ID_GAMES_ + MAX_GAMES, &ContentStudioView::OnUpdateGames)
    ON_COMMAND(ID_TESTRENDERMODES_WIREFRAMERENDERING, &ContentStudioView::OnTestrendermodesWireframerendering)
    ON_COMMAND(ID_TESTRENDERMODES_SOLIDRENDERING, &ContentStudioView::OnTestrendermodesSolidrendering)
    ON_UPDATE_COMMAND_UI(ID_TESTRENDERMODES_WIREFRAMERENDERING, &ContentStudioView::OnUpdateTestrendermodesWireframerendering)
    ON_UPDATE_COMMAND_UI(ID_TESTRENDERMODES_SOLIDRENDERING, &ContentStudioView::OnUpdateTestrendermodesSolidrendering)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void ContentStudioViewRenderCallback(void* context)
{
    ContentStudioView* pView = (ContentStudioView*)context;
    if (pView)
    {
        pView->OnRenderCallback();
        pView->OnRenderOverlayCallback(false);
    }
}

ContentStudioView::ContentStudioView() :
    _mainFrame(nullptr),
    _indexToGraphicsComponent(0),
    _indexToGamesComponent(0)
{
    _archiveContentPreviewCamera.SetViewParams(Vector3(0, 0, -100), Vector3(0, 0, 0), Vector3(0, 1, 0));
    _worldCamera.SetViewParams(Vector3(0, 0, -150), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

ContentStudioView::~ContentStudioView()
{
    SetViewNotificationHandlers(nullptr);
}

BOOL ContentStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void ContentStudioView::OnDraw(CDC* pDC)
{
    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    UNREFERENCED_PARAMETER(pDC);
    ContentStudioViewRenderCallback(this);
}

BOOL ContentStudioView::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return FALSE;
}

void ContentStudioView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void ContentStudioView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}

RenderingMode ContentStudioView::GetCurrentRenderingMode()
{
    ContentStudioApp* pApp = (ContentStudioApp*)AfxGetApp();
    if (pApp)
    {
        return pApp->GetCurrentRenderingMode();
    }

    return RenderingMode::RenderWireframe;
}

#ifdef _DEBUG
void ContentStudioView::AssertValid() const
{
    CView::AssertValid();
}

void ContentStudioView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

ContentStudioDoc* ContentStudioView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ContentStudioDoc)));
    return (ContentStudioDoc*)m_pDocument;
}

MainFrame* ContentStudioView::GetMainFrame() const
{
    ASSERT(_mainFrame->IsKindOf(RUNTIME_CLASS(MainFrame)));
    return (MainFrame*)_mainFrame;
}
#endif

void ContentStudioView::OnNcDestroy()
{
    // Free any created graphics device
    _graphics = nullptr;
    if (_mainFrame)
    {
        _mainFrame->GetArchivePreviewRenderingPane()->SetGraphicsDevice(nullptr);
    }
    CView::OnNcDestroy();
}

void ContentStudioView::SetViewNotificationHandlers(INotifyView* notifyView)
{
    // Set view notification handlers for all particpating controls and objects
    GetDocument()->SetViewNotificationHandler(notifyView);
    if (_mainFrame)
    {
        _mainFrame->GetArchivePane()->SetViewNotificationHandler(notifyView);
    }
}

void ContentStudioView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
        
    // Create overlay window if needed
    if (_transparentWindow.m_hWnd == nullptr)
    {
        _transparentWindow.CreateEx(WS_EX_LAYERED, _T("STATIC"), NULL, WS_POPUP | WS_VISIBLE, -1, -1, 0, 0, m_hWnd, NULL);
        this->SetFocus();
    }

    _oleDropTarget.Register(this);

    //DragAcceptFiles();
    _mainFrame = (MainFrame*)(AfxGetApp()->GetMainWnd());

    // Clear notification handlers
    SetViewNotificationHandlers(nullptr);

    _indexToGraphicsComponent = INVALID_SELECTION_INDEX;
    _indexToGamesComponent = INVALID_SELECTION_INDEX;
    _indexToContentComponent = INVALID_SELECTION_INDEX;
    
    int currentGraphicsDevice = theApp.GetInt(_T("CurrentGraphicsDevice"), 0);
    int currentGame = theApp.GetInt(_T("CurrentGame"), 0);

    // Default to creating and selecting the first discovered components
    SwitchGraphicsDevice(currentGraphicsDevice);
    SwitchGame(currentGame);

    // Initialize views
    PopulateArchiveExplorer();

    // Let the application's main idle loop tick the view's main 3D
    // render loop.
    theApp.SetRenderCallback(ContentStudioViewRenderCallback, this);

    // Set notification handlers
    SetViewNotificationHandlers(this);
}

void ContentStudioView::PopulateArchiveExplorer()
{
    HRESULT hr = S_OK;
    ComPtr<GDK::Content::IContentFactory> contentFactory;
    ComPtr<IStringList> requiredResources;
    ComPtr<GDK::Content::IContent> content;

    IfNullGotoExit(GetDocument(), E_UNEXPECTED);
    IfNullGotoExit(_gameEdit, E_UNEXPECTED);
    
    // Populate the archive pane
    _mainFrame->GetArchivePane()->Reset();        
    IfFailedGotoExit(_gameEdit->GetRequiredResources(&requiredResources));
    for (size_t i = 0; i < requiredResources->GetCount(); ++i)
    {
        ComPtr<IString> resource;
        requiredResources->GetAt(i, resource.ReleaseAndGetAddressOf());
        _mainFrame->GetArchivePane()->AddRequiredResource(resource->GetBuffer());
    }

    _mainFrame->GetArchivePane()->SetArchiveName(theApp._components.at(theApp._zeroBasedGamesComponents[_indexToGamesComponent]).displayName);
    
    content = GetDocument()->GetContent();
    if (content)
    {
        
    }

Exit:

    return;
}

void ContentStudioView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    CView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void ContentStudioView::OnGraphics(UINT id)
{
    SwitchGraphicsDevice(id - ID_GRAPHICS_);    
}

void ContentStudioView::OnUpdateGraphics(CCmdUI *pCmdUI)
{
    int rendererIndex = pCmdUI->m_nID - ID_GRAPHICS_;
    pCmdUI->SetCheck(rendererIndex == _indexToGraphicsComponent);
}

void ContentStudioView::OnGames(UINT id)
{
    SwitchGame(id - ID_GAMES_);    
}

void ContentStudioView::OnUpdateGames(CCmdUI *pCmdUI)
{
    int gameIndex = pCmdUI->m_nID - ID_GAMES_;
    pCmdUI->SetCheck(gameIndex == _indexToGamesComponent);
}

void ContentStudioView::SwitchGraphicsDevice(size_t index)
{
    HRESULT hr = S_OK;
    if (index != _indexToGraphicsComponent)
    {
        ComPtr<IGraphicsDevice> graphicsDevice;
        
        // Create graphics device for rendering panes
        hr = theApp.CreateGraphicsDevice(index, _mainFrame->GetArchivePreviewRenderingPane()->GetRenderWindow(), &graphicsDevice);
        if SUCCEEDED(hr)
        {
            _mainFrame->GetArchivePreviewRenderingPane()->SetGraphicsDevice(graphicsDevice.Get());
        }

        // Create graphics device for main window
        hr = theApp.CreateGraphicsDevice(index, m_hWnd, graphicsDevice.ReleaseAndGetAddressOf());
        if SUCCEEDED(hr)
        {
            _graphics = graphicsDevice;
        }

        _indexToGraphicsComponent = index;
    }

    theApp.WriteInt(_T("CurrentGraphicsDevice"), (int)_indexToGraphicsComponent);
    OnContentSelectionChanged();
}

void ContentStudioView::SwitchGame(size_t index)
{
    HRESULT hr = S_OK;
    if (index != _indexToGamesComponent)
    {
        // Create Game Factory
        hr = theApp.CreateGameFactory(index, _gameFactory.ReleaseAndGetAddressOf());
        if (SUCCEEDED(hr))
        {
            // Create Game Edit
            GameCreationParameters parameters = {0};
            parameters.host = this;
            parameters.content = reinterpret_cast<ContentStudioDoc*>(this->_mainFrame->GetActiveDocument())->GetContent();
            parameters.graphicsDevice = _graphics.Get();

            hr = _gameFactory->CreateGameEdit(parameters, _gameEdit.ReleaseAndGetAddressOf());
            if (SUCCEEDED(hr))
            {
                DebugOut(L"Created IGameEdit Successfully\n");

                hr = _gameEdit.As(&_game);
                if (SUCCEEDED(hr))
                {
                    DebugOut(L"Obtained IGame Successfully\n");
                }
            }
        }

        _indexToGamesComponent = index;
    }
    theApp.WriteInt(_T("CurrentGame"), (int)_indexToGamesComponent);
}

void ContentStudioView::OnRenderCallback()
{
    RenderingMode renderingMode = theApp.GetCurrentRenderingMode();

    if (Keyboard::IsKeyJustPressed(VK_F5))
    {
        GetMainFrame()->EnterRunGameMode();
    }

    if (Keyboard::IsKeyJustPressed(VK_ESCAPE))
    {
        GetMainFrame()->ExitRunGameMode();
    }

    CRect clientRect;
    GetClientRect(&clientRect);

    _worldCamera.SetAspect((float)clientRect.Width()/(float)clientRect.Height());

    if (_game)
    {
        _game->Update(theApp.GetElapsedGameTime());
    }

    // Render main window
    if (_graphics)
    {
        Vector4 clearColor(0.39f, 0.58f, 0.929f, 0.0f);
        _graphics->Clear(clearColor);
        _graphics->ClearDepth(1.0f);
        if (_game)
        {
            switch(renderingMode)
            {
            case RenderSolid:
                _graphics->SetFillMode(GraphicsFillMode::Solid);
                break;

            case RenderWireframe:
                _graphics->SetFillMode(GraphicsFillMode::Wireframe);
                break;
            }

            _game->Draw(_worldCamera.View(), _worldCamera.Projection());
        }
        _graphics->Present();
    }

    // Render all panes
    if (_mainFrame)
    {
        _mainFrame->GetArchivePreviewRenderingPane()->SetCamera(&_archiveContentPreviewCamera);
        _mainFrame->GetArchivePreviewRenderingPane()->SetFillMode(renderingMode == RenderSolid ? GraphicsFillMode::Solid : GraphicsFillMode::Wireframe);
        _mainFrame->GetArchivePreviewRenderingPane()->Render();
    }
}

void ContentStudioView::OnRenderOverlayCallback(bool erase)
{
    CRect clientRect;
    GetClientRect(clientRect);
    ClientToScreen(clientRect);

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    POINT ptPos = clientRect.TopLeft();
    SIZE sizeWnd = clientRect.Size();
    POINT ptSrc = {0, 0};
    CDC memDC;
    CDC screenDC;
    CBitmap bitmap;
    CBitmap oldbitmap;

    screenDC.Attach(::GetDC(NULL));
    memDC.Attach(CreateCompatibleDC(screenDC.m_hDC));
    bitmap.Attach(CreateCompatibleBitmap(screenDC.m_hDC, clientRect.Width(), clientRect.Height()));
    oldbitmap.Attach((HBITMAP)SelectObject(memDC.m_hDC, bitmap));

    if (!erase)
    {
        OnDrawOverlays(&memDC);
    }

    if (_transparentWindow.m_hWnd != nullptr)
    {
        _transparentWindow.UpdateLayeredWindow(&screenDC, &ptPos, &sizeWnd, &memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    SelectObject(memDC.m_hDC, oldbitmap);
}

void ContentStudioView::DrawGradientBackground(CDC* pDC)
{
    CRect clientRect;
    GetClientRect(clientRect);
    Gdiplus::Graphics graphics(pDC->m_hDC);
    Gdiplus::LinearGradientBrush gradient(Gdiplus::Point(0,0), Gdiplus::Point(0,clientRect.Height()), Gdiplus::Color(255, 0, 0, 127), Gdiplus::Color(255, 127, 127, 255));
    graphics.FillRectangle(&gradient, 0, 0, clientRect.Width(), clientRect.Height());
}

void ContentStudioView::OnDrawOverlays(CDC* pDC)
{
    Gdiplus::Graphics graphics(pDC->m_hDC);

    // TODO: Render overlay graphics
}

void ContentStudioView::OnTestrendermodesWireframerendering()
{
    theApp.ChangeRenderingMode(RenderWireframe);
    Invalidate();
}

void ContentStudioView::OnTestrendermodesSolidrendering()
{
    theApp.ChangeRenderingMode(RenderSolid);
    Invalidate();
}

void ContentStudioView::OnUpdateTestrendermodesWireframerendering(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(theApp.GetCurrentRenderingMode() == RenderWireframe);
}

void ContentStudioView::OnUpdateTestrendermodesSolidrendering(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(theApp.GetCurrentRenderingMode() == RenderSolid);
}

HRESULT STDMETHODCALLTYPE ContentStudioView::QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown))
    {
        *ppvObject = this; // static_cast<IUnkown*>(this) does not compile?
        AddRef();
        return S_OK;
    }
    else if (riid == __uuidof(IGameHost))
    {
        *ppvObject = static_cast<IGameHost*>(this);
        AddRef();
        return S_OK;
    }
    else if (riid == __uuidof(INotifyView))
    {
        *ppvObject = static_cast<INotifyView*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ContentStudioView::AddRef(void)
{
    // Always return a non-zero ref count because lifetime is already managed
    // by the mainframe.
    return 1;
}

ULONG STDMETHODCALLTYPE ContentStudioView::Release(void)
{
    // Never delete this object because lifetime is already managed
    // by the mainframe.
    return 1;
}

void STDMETHODCALLTYPE ContentStudioView::OnDocumentChanged()
{
    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Figure out what changed and update the view
}

void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionOpened()
{
    ArchiveExplorerPane* explorerPane = _mainFrame->GetArchivePane();
    if (!explorerPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Figure out current selection and update the view
    std::wstring selectedContent;
    if (explorerPane->GetSelectedContent(selectedContent))
    {
        // HACKY HACK -- Looking for the .world extension may not be the most
        // reliable way to detect a world object launch, but it will work for
        // now.
        if (selectedContent.find(L".world") != std::wstring::npos)
        {
            DebugOut(L"Open WORLD Content '%ws' requested\n", selectedContent.c_str());
            
            // Get resource from document

            // Pass resource to be rendered to main view
        }        
    }
}

void STDMETHODCALLTYPE ContentStudioView::OnContentFrameChanged(_In_ bool incrementFrame)
{
    _mainFrame->GetArchivePreviewRenderingPane()->UpdateCurrentFrame(incrementFrame);
}

void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionChanged()
{
    ArchiveExplorerPane* explorerPane = _mainFrame->GetArchivePane();
    if (!explorerPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Figure out current selection and update the view
    std::wstring selectedContent;
    if (explorerPane->GetSelectedContent(selectedContent))
    {
        DebugOut(L"Selected Content = %ws\n", selectedContent.c_str());
        ComPtr<GDK::Content::IContent> content = GetDocument()->GetContent();
        if (content)
        {
            if (selectedContent.find(L".object") != std::wstring::npos)
            {
                _mainFrame->GetArchivePreviewRenderingPane()->SetTextureResource(nullptr);
                _mainFrame->GetArchivePreviewRenderingPane()->SetGeometryResource(nullptr);
                std::wstring geometryName = selectedContent + L"\\mesh.geometry";
                std::wstring textureName = selectedContent + L"\\primary.texture";;

                ComPtr<GDK::Content::IGeometryResource> geometryResource;
                if (SUCCEEDED(content->GetGeometryResource(geometryName.c_str(), &geometryResource)))
                {
                    uint32_t format = 0;
                    uint32_t numIndices = 0;
                    const uint32_t* indices = nullptr;
                    uint32_t numVertices = 0;
                    const GDK::Content::GEOMETRY_RESOURCE_VERTEX* vertices = nullptr;
                    geometryResource->GetFormat(&format);
                    geometryResource->GetIndices(&numIndices, &indices);
                    geometryResource->GetVertices(&numVertices, &vertices);
                    DebugOut(L"Geometry Resource Loaded: Format = 0x%lx, %d indices, %d vertices\n", format, numIndices, numVertices);
                }

                ComPtr<GDK::Content::ITextureResource> textureResource;
                if (SUCCEEDED(content->GetTextureResource(textureName.c_str(), &textureResource)))
                {
                    uint32_t format = 0;
                    uint32_t numImages = 0;
                    uint32_t imageWidth = 0;
                    uint32_t imageHeight = 0;

                    textureResource->GetFormat(&format);
                    textureResource->GetImageCount(&numImages);
                    DebugOut(L"Texture Resource Loaded: Format = 0x%lx, %d images\n", format, numImages);
                    for (uint32_t i = 0; i < numImages; i++)
                    {
                        textureResource->GetSize(i, &imageWidth, &imageHeight);
                        DebugOut(L"Image %d, (%dx%d)\n", i, imageWidth, imageHeight);
                    }
                }

                _mainFrame->GetArchivePreviewRenderingPane()->SetObjectResources(geometryResource.Get(), textureResource.Get());
            }

            if (selectedContent.find(L".geometry") != std::wstring::npos)
            {
                _mainFrame->GetArchivePreviewRenderingPane()->SetTextureResource(nullptr);
                _mainFrame->GetArchivePreviewRenderingPane()->SetGeometryResource(nullptr);

                ComPtr<GDK::Content::IGeometryResource> geometryResource;
                if (SUCCEEDED(content->GetGeometryResource(selectedContent.c_str(), &geometryResource)))
                {
                    uint32_t format = 0;
                    uint32_t numIndices = 0;
                    const uint32_t* indices = nullptr;
                    uint32_t numVertices = 0;
                    const GDK::Content::GEOMETRY_RESOURCE_VERTEX* vertices = nullptr;
                    geometryResource->GetFormat(&format);
                    geometryResource->GetIndices(&numIndices, &indices);
                    geometryResource->GetVertices(&numVertices, &vertices);
                    DebugOut(L"Geometry Resource Loaded: Format = 0x%lx, %d indices, %d vertices\n", format, numIndices, numVertices);

                    _mainFrame->GetArchivePreviewRenderingPane()->SetGeometryResource(geometryResource.Get());
                }
            }

            if (selectedContent.find(L".texture") != std::wstring::npos)
            {
                _mainFrame->GetArchivePreviewRenderingPane()->SetTextureResource(nullptr);
                _mainFrame->GetArchivePreviewRenderingPane()->SetGeometryResource(nullptr);

                ComPtr<GDK::Content::ITextureResource> textureResource;
                if (SUCCEEDED(content->GetTextureResource(selectedContent.c_str(), &textureResource)))
                {
                    uint32_t format = 0;
                    uint32_t numImages = 0;
                    uint32_t imageWidth = 0;
                    uint32_t imageHeight = 0;

                    textureResource->GetFormat(&format);
                    textureResource->GetImageCount(&numImages);
                    DebugOut(L"Texture Resource Loaded: Format = 0x%lx, %d images\n", format, numImages);
                    for (uint32_t i = 0; i < numImages; i++)
                    {
                        textureResource->GetSize(i, &imageWidth, &imageHeight);
                        DebugOut(L"Image %d, (%dx%d)\n", i, imageWidth, imageHeight);
                    }

                    _mainFrame->GetArchivePreviewRenderingPane()->SetTextureResource(textureResource.Get());
                }
            }
        }

        // Pass resource to be rendered to content preview
    }
}
DROPEFFECT ContentStudioView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    __super::OnDragOver(pDataObject, dwKeyState, point);
    return DROPEFFECT_COPY;
}

DROPEFFECT ContentStudioView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    STGMEDIUM stgm;
    if (pDataObject->GetData(CF_TEXT, &stgm))
    {
        return DROPEFFECT_COPY;
    }

    __super::OnDragEnter(pDataObject, dwKeyState, point);
    return DROPEFFECT_COPY;
}

void ContentStudioView::OnDragLeave()
{
    __super::OnDragLeave();
}

BOOL ContentStudioView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    STGMEDIUM stgm;
    if (pDataObject->GetData(CF_TEXT, &stgm))
    {
        if (stgm.hGlobal)
        {
            CHAR* pMem = (CHAR*)GlobalLock(stgm.hGlobal);
            if (pMem)
            {
                // Create object
            }
            GlobalUnlock(stgm.hGlobal);
            GlobalFree(stgm.hGlobal); // Should I free this?
        }
    }

    return __super::OnDrop(pDataObject, dropEffect, point);
}
