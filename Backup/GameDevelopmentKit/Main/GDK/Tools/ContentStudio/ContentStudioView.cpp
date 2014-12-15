#include "stdafx.h"
#include "ContentStudioView.h"

using namespace GDK;

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
    }
}

ContentStudioView::ContentStudioView() :
    _mainFrame(nullptr),
    _indexToGraphicsComponent(0),
    _indexToGamesComponent(0),
    _dropTargetRegistered(false)
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
        _mainFrame->GetContentPreviewPane()->SetGraphicsDevice(nullptr);
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

    if (!_dropTargetRegistered)
    {
        _oleDropTarget.Register(this);
        _dropTargetRegistered = true;
    }

    _mainFrame = (MainFrame*)(AfxGetApp()->GetMainWnd());

    // Clear notification handlers
    SetViewNotificationHandlers(nullptr);

    _indexToGraphicsComponent = INVALID_SELECTION_INDEX;
    _indexToGamesComponent = INVALID_SELECTION_INDEX;

    int currentGraphicsDevice = theApp.GetInt(_T("CurrentGraphicsDevice"), 0);
    int currentGame = theApp.GetInt(_T("CurrentGame"), 0);

    // Default to creating and selecting the first discovered components
    SwitchGraphicsDevice(currentGraphicsDevice);
    SwitchGame(currentGame);

    // Initialize views
    PopulateArchiveExplorer();
    _mainFrame->GetContentPreviewPane()->SetTextureContent(nullptr);
    _mainFrame->GetContentPreviewPane()->SetGeometryContent(nullptr);

    // Let the application's main idle loop tick the view's main 3D
    // render loop.
    theApp.SetRenderCallback(ContentStudioViewRenderCallback, this);

    // Set notification handlers
    SetViewNotificationHandlers(this);
}

void ContentStudioView::PopulateArchiveExplorer()
{
    _mainFrame->GetArchivePane()->Reset();
    _mainFrame->GetArchivePane()->SetArchiveName(GetDocument()->GameName.c_str());

    for (size_t i = 0; i < GetDocument()->ContentList.size(); i++)
    {
        _mainFrame->GetArchivePane()->AddRequiredResource(GetDocument()->ContentList[i].c_str());
    }
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
    UNREFERENCED_PARAMETER(index);

    CRect clientRect;
    GetClientRect(&clientRect);

    _graphics = GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        GDK::GraphicsDevice::Type::DirectX, this->m_hWnd, 800, 600));

    _mainFrame->GetContentPreviewPane()->SetGraphicsDevice(GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        GDK::GraphicsDevice::Type::DirectX, _mainFrame->GetContentPreviewPane()->GetRenderWindow(),
        800, 600)));

    theApp.WriteInt(_T("CurrentGraphicsDevice"), (int)_indexToGraphicsComponent);
    OnContentSelectionChanged();
}

void ContentStudioView::SwitchGame(size_t index)
{
    UNREFERENCED_PARAMETER(index);
    _game = Quake2::Quake2Game::Create(GDK::Game::CreateParameters());
    theApp.WriteInt(_T("CurrentGame"), (int)_indexToGamesComponent);
}

void ContentStudioView::OnRenderCallback()
{
    RenderingMode renderingMode = theApp.GetCurrentRenderingMode();
    UNREFERENCED_PARAMETER(renderingMode);

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
        //_game->Update(theApp.GetElapsedGameTime());
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
                //_graphics->SetFillMode(GraphicsFillMode::Solid);
                break;

            case RenderWireframe:
                //_graphics->SetFillMode(GraphicsFillMode::Wireframe);
                break;
            }

            //_game->Draw(_worldCamera.View(), _worldCamera.Projection());
        }
        _graphics->Present();
    }

    // Render all panes
    if (_mainFrame)
    {
        _mainFrame->GetContentPreviewPane()->SetCamera(&_archiveContentPreviewCamera);
        //_mainFrame->GetContentPreviewPane()->SetFillMode(renderingMode == RenderSolid ? GraphicsFillMode::Solid : GraphicsFillMode::Wireframe);
        _mainFrame->GetContentPreviewPane()->Render();
    }
}

void ContentStudioView::DrawGradientBackground(CDC* pDC)
{
    CRect clientRect;
    GetClientRect(clientRect);
    Gdiplus::Graphics graphics(pDC->m_hDC);
    Gdiplus::LinearGradientBrush gradient(Gdiplus::Point(0,0), Gdiplus::Point(0,clientRect.Height()), Gdiplus::Color(255, 0, 0, 127), Gdiplus::Color(255, 127, 127, 255));
    graphics.FillRectangle(&gradient, 0, 0, clientRect.Width(), clientRect.Height());
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

void STDMETHODCALLTYPE ContentStudioView::OnDocumentChanged()
{
    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;
}

void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionOpened()
{
    ContentPane* explorerPane = _mainFrame->GetArchivePane();
    if (!explorerPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    std::wstring selectedContent;
    if (explorerPane->GetSelectedContent(selectedContent))
    {
        if (selectedContent.rfind(L".world") != std::wstring::npos)
        {
            // World content was double clicked
        }
    }
}

void STDMETHODCALLTYPE ContentStudioView::OnContentFrameChanged(_In_ bool incrementFrame)
{
    UNREFERENCED_PARAMETER(incrementFrame);
}

void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionChanged()
{
    ContentPane* explorerPane = _mainFrame->GetArchivePane();
    if (!explorerPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    std::wstring selectedContent;
    if (explorerPane->GetSelectedContent(selectedContent))
    {
        // Ensure that the preview pane is cleared of any previously set content
        _mainFrame->GetContentPreviewPane()->SetTextureContent(nullptr);
        _mainFrame->GetContentPreviewPane()->SetGeometryContent(nullptr);

        if (selectedContent.rfind(L".geometry") != std::wstring::npos)
        {
            _mainFrame->GetContentPreviewPane()->SetGeometryContent(
                    GDK::Content::LoadGeometryContent(selectedContent));
        }
        else if (selectedContent.rfind(L".texture") != std::wstring::npos)
        {
            _mainFrame->GetContentPreviewPane()->SetTextureContent(
                    GDK::Content::LoadTextureContent(selectedContent));
        }
        else if (selectedContent.rfind(L".object") != std::wstring::npos)
        {
            std::wstring geometryName = selectedContent + L"\\mesh.geometry";
            std::wstring textureName = selectedContent + L"\\primary.texture";;

            _mainFrame->GetContentPreviewPane()->SetObjectContent(
                GDK::Content::LoadGeometryContent(geometryName), 
                GDK::Content::LoadTextureContent(textureName));
        }
    }
}
DROPEFFECT ContentStudioView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    __super::OnDragOver(pDataObject, dwKeyState, point);

    if (IsSupportedDroppedContent(pDataObject))
    {
        return DROPEFFECT_COPY;
    }

    return DROPEFFECT_NONE;
}

DROPEFFECT ContentStudioView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    __super::OnDragEnter(pDataObject, dwKeyState, point);

    if (IsSupportedDroppedContent(pDataObject))
    {
        return DROPEFFECT_COPY;
    }

    return DROPEFFECT_NONE;
}

void ContentStudioView::OnDragLeave()
{
    __super::OnDragLeave();
}

BOOL ContentStudioView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    if (IsSupportedDroppedContent(pDataObject))
    {
        std::wstring droppedContent = GetDroppedContent(pDataObject);
        if (!droppedContent.empty())
        {
            ContentPane* explorerPane = _mainFrame->GetArchivePane();
            if (!explorerPane)
            {
                return FALSE;
            }

            std::wstring selectedContent;
            if (explorerPane->GetSelectedContent(selectedContent))
            {
                // Create object and add to world
            }
        }
    }

    return __super::OnDrop(pDataObject, dropEffect, point);
}

bool ContentStudioView::IsSupportedDroppedContent(COleDataObject* pDataObject)
{
    std::wstring droppedContent = GetDroppedContent(pDataObject);
    if (!droppedContent.empty())
    {
        if (droppedContent.rfind(L".object") != std::wstring::npos)
        {
            return true;
        }
    }

    return false;
}

std::wstring ContentStudioView::GetDroppedContent(COleDataObject* pDataObject)
{
    std::wstring droppedContent;

    if (!pDataObject)
    {
        return droppedContent;
    }

    STGMEDIUM stgm;
    if (pDataObject->GetData(CF_UNICODETEXT, &stgm))
    {
        if (stgm.hGlobal)
        {
            WCHAR* pMem = (WCHAR*)GlobalLock(stgm.hGlobal);
            if (pMem)
            {
                droppedContent = pMem;
            }
            GlobalUnlock(stgm.hGlobal);
        }
    }

    return droppedContent;
}
