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
    ON_COMMAND_RANGE(ID_AUDIO_, ID_AUDIO_ + MAX_RENDERERS, &ContentStudioView::OnAudio)
    ON_UPDATE_COMMAND_UI_RANGE(ID_AUDIO_, ID_AUDIO_ + MAX_RENDERERS, &ContentStudioView::OnUpdateAudio)
    ON_COMMAND_RANGE(ID_GAMES_, ID_GAMES_ + MAX_GAMES, &ContentStudioView::OnGames)
    ON_UPDATE_COMMAND_UI_RANGE(ID_GAMES_, ID_GAMES_ + MAX_GAMES, &ContentStudioView::OnUpdateGames)
    ON_COMMAND(ID_TESTRENDERMODES_WIREFRAMERENDERING, &ContentStudioView::OnTestrendermodesWireframerendering)
    ON_COMMAND(ID_TESTRENDERMODES_SOLIDRENDERING, &ContentStudioView::OnTestrendermodesSolidrendering)
    ON_COMMAND(ID_WORLD_SAVE, &ContentStudioView::OnSaveWorld)
    ON_COMMAND(ID_WORLD_NEW, &ContentStudioView::OnNewWorld)
    ON_COMMAND(ID_WORLD_SAVEAS, &ContentStudioView::OnSaveWorldAs)
    ON_COMMAND(ID_EDIT_WORLD_RENAME, &ContentStudioView::OnWorldRename)
    ON_UPDATE_COMMAND_UI(ID_TESTRENDERMODES_WIREFRAMERENDERING, &ContentStudioView::OnUpdateTestrendermodesWireframerendering)
    ON_UPDATE_COMMAND_UI(ID_TESTRENDERMODES_SOLIDRENDERING, &ContentStudioView::OnUpdateTestrendermodesSolidrendering)
    ON_WM_KEYDOWN()
    ON_WM_SETCURSOR()
    ON_COMMAND(ID_EDIT_COPY, &ContentStudioView::OnEditCopy)
    ON_COMMAND(ID_EDIT_PASTE, &ContentStudioView::OnEditPaste)
    ON_COMMAND(ID_EDIT_CUT, &ContentStudioView::OnEditCut)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

_Use_decl_annotations_
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
    _dropTargetRegistered(false),
    _processAddContent(false),
    _contentListCount(0),
    _graphicsType(GDK::GraphicsDevice::Type::DirectX),
    _audioType(GDK::AudioDevice::Type::OpenAL),
    _draggingObjectFromContentPane(false)
{
    ResetContentPreviewCamera();
    ResetWorldCamera();
}

ContentStudioView::~ContentStudioView()
{
    SetViewNotificationHandlers(nullptr);
}

void ContentStudioView::ResetContentPreviewCamera()
{
    //_contentPreviewPaneCamera.SetViewParams(Vector3(0, 0, 100), Vector3::Zero(), Vector3::Up());
}

void ContentStudioView::OnWorldRename()
{
    GetDocument()->RenameWorld();
}

void ContentStudioView::ResetWorldCamera()
{
    _worldCamera.SetViewParams(Vector3(0, 100, -150), Vector3::Zero(), Vector3::Up());
    _worldCamera.SetSpeed(500.0f);
}

_Use_decl_annotations_
BOOL ContentStudioView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

_Use_decl_annotations_
void ContentStudioView::OnDraw(CDC* pDC)
{
    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    UNREFERENCED_PARAMETER(pDC);
    ContentStudioViewRenderCallback(this);
}

_Use_decl_annotations_
BOOL ContentStudioView::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return FALSE;
}

_Use_decl_annotations_
void ContentStudioView::OnRButtonUp(UINT /* nFlags */, CPoint /*point*/)
{
    //ClientToScreen(&point);
    //OnContextMenu(this, point);
}

_Use_decl_annotations_
void ContentStudioView::OnContextMenu(CWnd* /* pWnd */, CPoint /*point*/)
{
    //theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
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

_Use_decl_annotations_
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
    if (_mainFrame)
    {
        ASSERT(_mainFrame->IsKindOf(RUNTIME_CLASS(MainFrame)));
    }
    return (MainFrame*)_mainFrame;
}
#endif

void ContentStudioView::OnNcDestroy()
{
    // Free any created graphics device
    _selectedGameObject = nullptr;
    _copiedGameObject = nullptr;
    _graphics = nullptr;
    _audio = nullptr;
    if (_mainFrame)
    {
        _mainFrame->GetContentPreviewPane()->SetGraphicsDevice(nullptr);
    }

    if (_oleDropTarget)
    {
        _oleDropTarget->Revoke();
        if (_oleDropTarget->m_dwRef > 1)
        {
            _oleDropTarget->InternalRelease();
        }
    }

    if (GetDocument()->IsWorldModified())
    {
        if (AfxMessageBox(L"Would you like to save your changes?", MB_YESNO) == IDYES)
        {
            GetDocument()->SaveWorld(false);
        }
    }

    CView::OnNcDestroy();
}

_Use_decl_annotations_
void ContentStudioView::SetViewNotificationHandlers(INotifyView* notifyView)
{
    // Set view notification handlers for all particpating controls and objects
    GetDocument()->SetViewNotificationHandler(notifyView);
    if (_mainFrame)
    {
        _mainFrame->GetContentPane()->SetViewNotificationHandler(notifyView);
    }
}

void ContentStudioView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    if (!_oleDropTarget)
    {
        _oleDropTarget = std::make_shared<COleDropTarget>();
        _oleDropTarget->Register(this);
    }

    _mainFrame = (MainFrame*)(AfxGetApp()->GetMainWnd());

    _contentListCount = 0;
    _processAddContent = false;

    // Clear notification handlers
    SetViewNotificationHandlers(nullptr);

    _indexToGraphicsComponent = INVALID_SELECTION_INDEX;
    _indexToGamesComponent = INVALID_SELECTION_INDEX;

    int currentGraphicsDevice = theApp.GetInt(_T("CurrentGraphicsDevice"), (int)GDK::GraphicsDevice::Type::DirectX);
    SwitchGraphicsDevice((GDK::GraphicsDevice::Type)currentGraphicsDevice);

    int currentAudioDevice = theApp.GetInt(_T("CurrentAudioDevice"), (int)GDK::AudioDevice::Type::OpenAL);
    SwitchAudioDevice((GDK::AudioDevice::Type)currentAudioDevice);

    GetDocument()->CreateDefaultWorld(_graphics);

    // Initialize views
    PopulateContentPane(true);
    _mainFrame->GetContentPreviewPane()->SetGeometryContent(nullptr);
    _mainFrame->GetContentPreviewPane()->SetTextureContent(nullptr);

    // Let the application's main idle loop tick the view's main 3D
    // render loop.
    theApp.SetRenderCallback(ContentStudioViewRenderCallback, this);

    // Set notification handlers
    SetViewNotificationHandlers(this);
}

_Use_decl_annotations_
void ContentStudioView::PopulateContentPane(bool clearAll)
{
    if (clearAll)
    {
        _mainFrame->GetContentPane()->Reset();
        _mainFrame->GetContentPane()->SetArchiveName(GetDocument()->GameName.c_str());
    }

    for (size_t i = 0; i < GetDocument()->ContentPaneData.size(); i++)
    {
        _mainFrame->GetContentPane()->AddContent(GetDocument()->ContentPaneData[i]);
    }
    GetDocument()->ContentPaneData.clear();
}

_Use_decl_annotations_
void ContentStudioView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    CView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

_Use_decl_annotations_
void ContentStudioView::OnGraphics(UINT id)
{
    SwitchGraphicsDevice((GDK::GraphicsDevice::Type)(id - ID_GRAPHICS_));
}

_Use_decl_annotations_
void ContentStudioView::OnUpdateGraphics(CCmdUI *pCmdUI)
{
    GDK::GraphicsDevice::Type graphicsType = (GDK::GraphicsDevice::Type)(pCmdUI->m_nID - ID_GRAPHICS_);
    pCmdUI->SetCheck(graphicsType == _graphicsType);
}

_Use_decl_annotations_
void ContentStudioView::OnAudio(UINT id)
{
    SwitchAudioDevice((GDK::AudioDevice::Type)(id - ID_AUDIO_));
}

_Use_decl_annotations_
void ContentStudioView::OnUpdateAudio(CCmdUI *pCmdUI)
{
    GDK::AudioDevice::Type audioType = (GDK::AudioDevice::Type)(pCmdUI->m_nID - ID_AUDIO_);
    pCmdUI->SetCheck(audioType == _audioType);
}

_Use_decl_annotations_
void ContentStudioView::OnGames(UINT id)
{
    SwitchGame(id - ID_GAMES_);
}

_Use_decl_annotations_
void ContentStudioView::OnUpdateGames(CCmdUI *pCmdUI)
{
    int gameIndex = pCmdUI->m_nID - ID_GAMES_;
    pCmdUI->SetCheck(gameIndex == _indexToGamesComponent);
}

_Use_decl_annotations_
void ContentStudioView::SwitchAudioDevice(GDK::AudioDevice::Type type)
{
    if (_audio)
    {
        _audio->StopAllLoopingClips();
    }
    _audio = GDK::AudioDevice::Create(type);

    theApp.WriteInt(_T("CurrentAudioDevice"), (int)type);
    _audioType = type;
}

_Use_decl_annotations_
void ContentStudioView::SwitchGraphicsDevice(GDK::GraphicsDevice::Type type)
{
    CRect clientRect;
    GetClientRect(&clientRect);

    _selectedGameObject = nullptr;
    _copiedGameObject = nullptr;

    _mainFrame->GetContentPreviewPane()->SetGraphicsDevice(GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        type, _mainFrame->GetContentPreviewPane()->GetRenderWindow(),
        800, 600)));

    _graphics = GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        type, this->m_hWnd, 1280, 720));

    theApp.WriteInt(_T("CurrentGraphicsDevice"), (int)type);
    _graphicsType = type;

    _gizmo = Gizmo::Create(_graphics);

    ContentPaneTreeData selectedContent;
    if (_mainFrame->GetContentPane()->GetSelectedContent(selectedContent))
    {
        OnContentSelectionChanged(selectedContent);
    }
}

_Use_decl_annotations_
void ContentStudioView::SwitchGame(size_t index)
{
    _indexToGamesComponent = index;
    GDK::DeviceContext deviceContext;
    deviceContext.graphicsDevice = _graphics;
    deviceContext.audioDevice = _audio;

    _game = Game::Create(deviceContext, GetDocument()->GameWorld->SaveToContent());
    theApp.WriteInt(_T("CurrentGame"), (int)_indexToGamesComponent);
}

void ContentStudioView::OnRenderCallback()
{
    if (!GetMainFrame())
        return;

    if (GetFocus() == this)
    {
        UpdateViewInput();
        _worldCamera.Update(theApp.GetElapsedGameTime());
    }

    if (GetFocus() == GetMainFrame()->GetContentPreviewPane())
    {
        _contentPreviewPaneCamera.Update(theApp.GetElapsedGameTime());
    }

    // Process game play toggle in/out keys
    if (GetFocus() == this && (!Keyboard::IsKeyHeld(VK_SHIFT) && Keyboard::IsKeyJustPressed(VK_F5)) && !GetMainFrame()->IsGameRunning())
    {
        // Save current world being edited
        // GetDocument()->GameWorld
        ShowCursor(FALSE);
        Input::SetWindow(m_hWnd);
        Input::LockMouseCursor();
        GetMainFrame()->EnterRunGameMode();
        SwitchGame(0);
    } 
    
    if (GetFocus() == this && (Keyboard::IsKeyHeld(VK_SHIFT) && Keyboard::IsKeyJustPressed(VK_F5)) && GetMainFrame()->IsGameRunning())
    {
        ShowCursor(TRUE);
        Input::UnlockMouseCursor();
        GetMainFrame()->ExitRunGameMode();
        _game = nullptr;
    }

    if (GetFocus() == GetMainFrame()->GetContentPreviewPane())
    {
        // Process special content preview zoom keys
        if (Keyboard::IsKeyDown(VK_OEM_MINUS) || Keyboard::IsKeyDown(VK_OEM_PLUS))
        {
            auto forward = _contentPreviewPaneCamera.World().GetForward();
            if (Keyboard::IsKeyDown(VK_OEM_MINUS))
            {
                _contentPreviewPaneCamera.Move(-forward * 40 * theApp.GetElapsedGameTime());
            }
            else
            {
                _contentPreviewPaneCamera.Move(forward * 40 * theApp.GetElapsedGameTime());
            }
        }
    }

    CRect clientRect;
    GetClientRect(&clientRect);
    _worldCamera.SetAspect((float)clientRect.Width()/(float)clientRect.Height());

    if (GetMainFrame()->IsGameRunning() && _game)
    {
        if (Mouse::IsButtonDown(VK_LBUTTON))
        {
            GDK::Input::OnButtonDown(GDK::Button::MouseLeft);
        }

        if (Mouse::IsButtonDown(VK_MBUTTON))
        {
            GDK::Input::OnButtonDown(GDK::Button::MouseMiddle);
        }

        if (Mouse::IsButtonDown(VK_RBUTTON))
        {
            GDK::Input::OnButtonDown(GDK::Button::MouseRight);
        }

        if (Mouse::IsButtonJustReleased(VK_LBUTTON))
        {
            GDK::Input::OnButtonUp(GDK::Button::MouseLeft);
        }

        if (Mouse::IsButtonJustReleased(VK_MBUTTON))
        {
            GDK::Input::OnButtonUp(GDK::Button::MouseMiddle);
        }

        if (Mouse::IsButtonJustReleased(VK_RBUTTON))
        {
            GDK::Input::OnButtonUp(GDK::Button::MouseRight);
        }

        if (_game->Update(GDK::GameTime(theApp.GetElapsedGameTime(), theApp.GetTotalElapsedGameTime())) == GDK::UpdateResult::Exit)
        {
            // Force a return to edit mode and delete the game
            ShowCursor(TRUE);
            Input::UnlockMouseCursor();
            GetMainFrame()->ExitRunGameMode();
            _game = nullptr;
        }
    }

    if (_graphics)
    {
        Vector4 clearColor(0.39f, 0.58f, 0.929f, 0.0f);
        if (GetMainFrame()->IsGameRunning())
        {
            clearColor = Vector4::Zero();
        }
        
        _graphics->Clear(clearColor);
        _graphics->ClearDepth(1.0f);
     
        if (GetMainFrame()->IsGameRunning() && _game)
        {
            _game->Draw();
        }
        else
        {
            // Render GameWorld being edited
            if (GetDocument()->GameWorld)
            {
                _graphics->SetViewProjection(_worldCamera.View(), _worldCamera.Projection());
                GetDocument()->GameWorld->Draw(_worldCamera.View(), _worldCamera.Projection());

                if (_selectedGameObject)
                {
                    GDK::Matrix world = GDK::Matrix::CreateTranslation(_selectedGameObject->GetTransform().GetPosition());
                    _gizmo->Draw(world, _worldCamera, clientRect.Width());
                }
            }
        }

        _graphics->Present();
    }

    // Render content preview
    if (_mainFrame)
    {
        _mainFrame->GetContentPreviewPane()->SetCamera(&_contentPreviewPaneCamera);
        _mainFrame->GetContentPreviewPane()->Render();
    }
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

void ContentStudioView::OnSaveWorld()
{
    GetDocument()->SaveWorld(false);
}

void ContentStudioView::OnNewWorld()
{
    GetDocument()->CreateDefaultWorld(_graphics);
}

void ContentStudioView::OnSaveWorldAs()
{
    GetDocument()->SaveWorld(true);
}

_Use_decl_annotations_
void ContentStudioView::OnUpdateTestrendermodesWireframerendering(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(theApp.GetCurrentRenderingMode() == RenderWireframe);
}

_Use_decl_annotations_
void ContentStudioView::OnUpdateTestrendermodesSolidrendering(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(theApp.GetCurrentRenderingMode() == RenderSolid);
}

void STDMETHODCALLTYPE ContentStudioView::OnDocumentChanged()
{
    _processAddContent = true;
}

void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionOpened()
{
    ContentPane* contentPane = _mainFrame->GetContentPane();
    if (!contentPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    ContentPaneTreeData selectedContent;
    if (contentPane->GetSelectedContent(selectedContent))
    {
        ResetWorldCamera();

        if (selectedContent.contentPath.rfind(L".world") != std::wstring::npos)
        {
            GetDocument()->OpenWorld(_graphics, selectedContent.filePath);
        }
        else if (selectedContent.contentPath.rfind(L".bsp") != std::wstring::npos ||
            selectedContent.contentPath.rfind(L"Wolf") != std::wstring::npos)
        {
            std::vector<std::wstring> texturesList;
            std::vector<std::wstring> geometryList;
            std::vector<GDK::VisualInfo> visualInfos;
            contentPane->GetSelectedBSPGeometryAndTextureContent(geometryList, texturesList);
            for (size_t i = 0; i < texturesList.size(); i++)
            {
                visualInfos.push_back(GDK::VisualInfo(Matrix::Identity(), geometryList[i], texturesList[i]));
            }
            GetDocument()->GameWorld->CreateModel(visualInfos);
            GetDocument()->SetModified(true);
        }
    }
}

_Use_decl_annotations_
void STDMETHODCALLTYPE ContentStudioView::OnContentExpanded(const ContentPaneTreeData& selectedContent)
{
    GetDocument()->FindTopLevelContent(selectedContent.contentMapping, selectedContent.contentMappingRoot ,selectedContent.filePath, selectedContent.priority);
    PopulateContentPane(false);
}

_Use_decl_annotations_
void STDMETHODCALLTYPE ContentStudioView::OnContentSelectionChanged(const ContentPaneTreeData& selectedContent)
{
    ContentPane* contentPane = _mainFrame->GetContentPane();
    if (!contentPane)
    {
        return;
    }

    ContentStudioDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Reset camera
    ResetContentPreviewCamera();

    if (selectedContent.fileBasedContent)
    {
        // Figure out current selection and update the view
        if (selectedContent.contentPath.rfind(L".geometry") != std::wstring::npos)
        {
            _mainFrame->GetContentPreviewPane()->SetGeometryContent(selectedContent);
        }
        else if (selectedContent.contentPath.rfind(L".texture") != std::wstring::npos)
        {
            _mainFrame->GetContentPreviewPane()->SetTextureContent(
                GDK::Content::LoadTextureContent(selectedContent.contentPath));
        }
        else if (selectedContent.contentPath.rfind(L".wav") != std::wstring::npos)
        {
            std::shared_ptr<GDK::AudioContent> audioContent = GDK::Content::LoadAudioContent(selectedContent.contentPath);
            std::shared_ptr<GDK::AudioClip> audioClip = _audio->CreateAudioClip(audioContent);
            _audio->PlayClip(audioClip);
        }
    }
    else
    {
#if 0 // TODO
        auto& visualInfos = GDK::GameObject::GetVisualInfosForClass(selectedContent.contentPath);
        if (visualInfos.empty() || visualInfos[0].geometry.empty() || visualInfos[0].texture.empty())
        {
            _mainFrame->GetContentPreviewPane()->SetObjectContent(nullptr, nullptr);
        }
        else
        {
            _mainFrame->GetContentPreviewPane()->SetObjectContent(GDK::Content::LoadGeometryContent(visualInfos[0].geometry), GDK::Content::LoadTextureContent(visualInfos[0].texture));
        }
#endif
    }
}

_Use_decl_annotations_
void ContentStudioView::OnCreateObject(const std::wstring& name)
{
    std::map<std::wstring, std::wstring> properties;
    properties[L"classname"] = name;
    GetDocument()->GameWorld->CreateObjectEdit(properties);
}

_Use_decl_annotations_
bool ContentStudioView::GetScreenRaycast(const CPoint& point, Ray* ray)
{
    CRect clientRect;
    GetClientRect(&clientRect);

    return ScreenRaycast(point.x / (float)clientRect.Width(), point.y / (float)clientRect.Height(), _worldCamera.View(), _worldCamera.Projection(), ray);
}

_Use_decl_annotations_
DROPEFFECT ContentStudioView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    __super::OnDragOver(pDataObject, dwKeyState, point);
    if (IsSupportedDroppedContent(pDataObject))
    {
        _draggingObjectFromContentPane = true;

        if (_selectedGameObject)
        {
            OnRenderCallback();

            GDK::Ray ray;
            if (GetScreenRaycast(point, &ray))
            {
                GDK::Plane plane(_selectedGameObject->GetTransform().GetPosition(), _worldCamera.World().GetForward());

                GDK::RaycastResult result;
                if (Raycast(&ray, nullptr, &plane, nullptr, &result))
                {
                    _selectedGameObject->GetTransform().SetPosition(result.point);
                }
            }
        }
        return DROPEFFECT_COPY;
    }
    else
    {
        _draggingObjectFromContentPane = false;
    }

    return DROPEFFECT_NONE;
}

_Use_decl_annotations_
DROPEFFECT ContentStudioView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    __super::OnDragEnter(pDataObject, dwKeyState, point);
    if (IsSupportedDroppedContent(pDataObject))
    {
        _draggingObjectFromContentPane = true;

        ContentPaneTreeData selectedContent;
        ContentPane* contentPane = _mainFrame->GetContentPane();
        if (contentPane->GetSelectedContent(selectedContent))
        {
            if (!selectedContent.fileBasedContent)
            {
                std::map<std::wstring, std::wstring> properties;
                properties[L"classname"] = selectedContent.contentPath;
                _selectedGameObject = GetDocument()->GameWorld->CreateObjectEdit(properties);
                GetDocument()->SetModified(true);
            }
            else if (selectedContent.contentPath.rfind(L".geometry") != std::wstring::npos)
            {
                std::vector<GDK::VisualInfo> visualInfos;

                visualInfos.push_back(VisualInfo(
                    Matrix::Identity(), 
                    selectedContent.contentPath,
                    (std::IsEncodedTextureContentPath(selectedContent.contentPath) 
                        ? std::GetTextureContentPathFromGeometryContentPath(selectedContent.contentPath)
                        : L"")));

                _selectedGameObject = GetDocument()->GameWorld->CreateModel(visualInfos);
                GetDocument()->SetModified(true);
            }

            GDK::Ray ray;
            if (GetScreenRaycast(point, &ray))
            {
                GDK::Plane plane(_worldCamera.World().GetTranslation() + _worldCamera.World().GetForward() * 300.0f, _worldCamera.World().GetForward());

                GDK::RaycastResult result;
                if (Raycast(&ray, nullptr, &plane, nullptr, &result))
                {
                    _selectedGameObject->GetTransform().SetPosition(result.point);
                }
            }

            OnRenderCallback();
        }

        return DROPEFFECT_COPY;
    }
    else
    {
        _draggingObjectFromContentPane = false;
    }

    return DROPEFFECT_NONE;
}

void ContentStudioView::OnDragLeave()
{
    if (_selectedGameObject)
    {
        GetDocument()->GameWorld->RemoveObject(_selectedGameObject);
        _selectedGameObject = nullptr;
        OnRenderCallback();
    }

    _draggingObjectFromContentPane = false;

    __super::OnDragLeave();
}

_Use_decl_annotations_
BOOL ContentStudioView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    if (IsSupportedDroppedContent(pDataObject))
    {
        std::wstring droppedContent = GetDroppedContent(pDataObject);
        if (!droppedContent.empty())
        {
            ContentPane* contentPane = _mainFrame->GetContentPane();
            if (!contentPane)
            {
                return FALSE;
            }

            ContentPaneTreeData selectedContent;
            if (contentPane->GetSelectedContent(selectedContent))
            {
                OnRenderCallback();
            }
        }
    }

    _draggingObjectFromContentPane = false;

    return __super::OnDrop(pDataObject, dropEffect, point);
}

_Use_decl_annotations_
bool ContentStudioView::IsSupportedDroppedContent(COleDataObject* pDataObject)
{
    std::wstring droppedContent = GetDroppedContent(pDataObject);
    if (!droppedContent.empty())
    {
#if 0 // TODO
        std::vector<std::wstring> supportedContent = GDK::GameObject::GetRegisteredClasses();
        for (size_t i = 0; i < supportedContent.size(); i++)
        {
            if (droppedContent == supportedContent[i] || droppedContent.rfind(L".geometry") != std::wstring::npos)
            {
                return true;
            }
        }
#endif
    }

    return false;
}

_Use_decl_annotations_
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

void ContentStudioView::UpdateViewInput()
{
    if(GetMainFrame()->IsGameRunning())
    {
        return;
    }

    // Avoid modifiers when dragging an object from the preview pane
    if (_draggingObjectFromContentPane)
    {
        return;
    }

    // Process world object delete
    if (Keyboard::IsKeyJustPressed(VK_DELETE))
    {
        if (_selectedGameObject)
        {
            GetDocument()->GameWorld->RemoveObject(_selectedGameObject);
            _selectedGameObject = nullptr;
        }
    }

    // Process world object origin center command
    if (Keyboard::IsKeyJustPressed(VK_SPACE))
    {
        if (_selectedGameObject)
        {
            _selectedGameObject->GetTransform().SetPosition(GDK::Vector3::Zero());
            _selectedGameObject->GetTransform().SetRotation(0);
        }
    }

    // Process object picking
    if (Mouse::IsButtonJustPressed(VK_LBUTTON))
    {
        // Capture the mouse position in the view and use it as the starting pick/drag
        // point.
        _startMousePoint = Mouse::GetPosition();
        
        CPoint startPos((int)_startMousePoint.x, (int)_startMousePoint.y);
        ScreenToClient(&startPos);

        GDK::Ray ray;
        if (GetScreenRaycast(startPos, &ray))
        {
            if (!_gizmo->Pick(ray))
            {
                // Missed the gizmo, so test if we hit an object
                _selectedGameObject = GetDocument()->GameWorld->PickClosestObject(ray);
            }
        }
    }

    if (Mouse::IsButtonJustReleased(VK_LBUTTON))
    {
        // Release any translation state and clear captured mouse start position
        _startMousePoint = GDK::Vector2::Zero();
        _gizmo->SetEditMode(EditMode::None);
    }

    Vector2 mouseDelta = Mouse::GetPositionDelta();
    if (Mouse::IsButtonHeld(VK_LBUTTON) && Keyboard::IsKeyHeld(VK_SHIFT))
    {
        // rotate mode
        if (_selectedGameObject)
        {
            _gizmo->SetEditMode(EditMode::Rotate);
            float currentRotation = _selectedGameObject->GetTransform().GetRotation();
            _selectedGameObject->GetTransform().SetRotation(currentRotation - mouseDelta.x * .05f);
            GetDocument()->SetModified(true);
        }
        return;
    }

    if (Mouse::IsButtonHeld(VK_RBUTTON))
    {
        /*
        CRect screenClientRect;
        GetClientRect(&screenClientRect);
        ClientToScreen(&screenClientRect);
        SetCursorPos(screenClientRect.left + screenClientRect.Width()/2, screenClientRect.top + screenClientRect.Height()/2);
        */
    }

    // If the mouse button is down, then assume we are dragging things around the view
    if (Mouse::IsButtonHeld(VK_LBUTTON))
    {
        // translate mode
        if (_selectedGameObject)
        {
            GDK::Vector3 currentPosition = _selectedGameObject->GetTransform().GetPosition();

            CPoint screenPos((int)Mouse::GetPosition().x, (int)Mouse::GetPosition().y);
            ScreenToClient(&screenPos);

            CPoint startScreenPos((int)_startMousePoint.x, (int)_startMousePoint.y);
            ScreenToClient(&startScreenPos);

            if (Keyboard::IsKeyHeld(VK_CONTROL) || _gizmo->GetEditMode() == EditMode::TranslateY)
            {
                currentPosition.y -= Mouse::GetPositionDelta().y;
            }
            else
            {
                Vector2 stepDir;
                Vector2 delta = Mouse::GetPositionDelta();
                Vector3 cameraRight = _worldCamera.World().GetRight();
                Vector3 cameraDown = Vector3::Cross(cameraRight, _worldCamera.World().GetForward());

                switch(_gizmo->GetEditMode())
                {
                case EditMode::None:
                    break;
                case EditMode::TranslateX:
                    stepDir = Vector2(
                        Vector3::Dot(Vector3::Right(), cameraRight), 
                        Vector3::Dot(Vector3::Right(), cameraDown));
                    stepDir.x = (fabsf(stepDir.x) > 0.3f) ? sign(stepDir.x) : 0;
                    stepDir.y = (fabsf(stepDir.y) > 0.3f) ? sign(stepDir.y) : 0;
                    currentPosition.x += stepDir.x * delta.x + stepDir.y * delta.y;
                    break;
                case EditMode::TranslateZ:
                    stepDir = Vector2(
                        Vector3::Dot(Vector3::Forward(), cameraRight), 
                        Vector3::Dot(Vector3::Forward(), cameraDown));
                    stepDir.x = (fabsf(stepDir.x) > 0.3f) ? sign(stepDir.x) : 0;
                    stepDir.y = (fabsf(stepDir.y) > 0.3f) ? sign(stepDir.y) : 0;
                    currentPosition.z += stepDir.x * delta.x + stepDir.y * delta.y;
                    break;
                }
            }

            _selectedGameObject->GetTransform().SetPosition(currentPosition);
            GetDocument()->SetModified(true);
        }
    }
}

_Use_decl_annotations_
BOOL ContentStudioView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return __super::OnSetCursor(pWnd, nHitTest, message);
}

void ContentStudioView::OnEditCopy()
{
    if (GetFocus() != this)
        return;

#if 0 // TODO: Fix this
    if (_selectedGameObject)
    {
        // Create a copy of the currently selected object
        _copiedGameObject = GetDocument()->GameWorld->CreateObject(_selectedGameObject->GetClassName());
        _copiedGameObject->GetTransform().SetPosition(_selectedGameObject->GetTransform().GetPosition());
        _copiedGameObject->GetTransform().SetRotation(_selectedGameObject->GetTransform().GetRotation());
    }
#endif
}

void ContentStudioView::OnEditPaste()
{
    if (GetFocus() != this)
        return;

#if 0 // TODO: Fix this
    // Copy the 'copied object' into the dragging object to make it the currently selected
    // object.
    if (_copiedGameObject)
    {
        _selectedGameObject = nullptr;
        _selectedGameObject = GetDocument()->GameWorld->CreateObject(_copiedGameObject->GetClassName());
        GDK::Vector3 dropPosition = _worldCamera.World().GetTranslation() + _worldCamera.World().GetForward() * 150;
        _selectedGameObject->GetTransform().SetPosition(dropPosition);
        _selectedGameObject->GetTransform().SetRotation(_copiedGameObject->GetTransform().GetRotation());
    }
#endif
}

void ContentStudioView::OnEditCut()
{
    if (GetFocus() != this)
        return;

#if 0 // TODO: Fix this
    if (_selectedGameObject)
    {
        // Create a copy of the currently selected object
        _copiedGameObject = GetDocument()->GameWorld->CreateObject(_selectedGameObject->GetTypeName());
        _copiedGameObject->GetTransform().SetPosition(_selectedGameObject->GetTransform().GetPosition());
        _copiedGameObject->GetTransform().SetRotation(_selectedGameObject->GetTransform().GetRotation());
        _selectedGameObject = nullptr;
    }
#endif
}

void ContentStudioView::OnClose()
{
    __super::OnClose();
}

_Use_decl_annotations_
LRESULT ContentStudioView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_KEYDOWN:
        {
            GDK::Button button;
            if (GDK::Input::GetButtonFromWin32Keycode(static_cast<byte_t>(wParam), &button))
            {
                GDK::Input::OnButtonDown(button);
            }
        }
        break;
    case WM_KEYUP:
        {
            GDK::Button button;
            if (GDK::Input::GetButtonFromWin32Keycode(static_cast<byte_t>(wParam), &button))
            {
                GDK::Input::OnButtonUp(button);
            }
        }
        break;
    default:
        break;
    }

    return __super::DefWindowProc(message, wParam, lParam);
}

void ContentStudioView::OnDestroy()
{
    __super::OnDestroy();
}
