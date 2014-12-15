#include "stdafx.h"
#include "ContentPreviewPane.h"

BEGIN_MESSAGE_MAP(ContentPreviewPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_CBN_SELCHANGE(IDC_ANIMATIONFRAMES_COMBOBOX, &ContentPreviewPane::OnSelchangeCombo1)
    ON_COMMAND(ID_PLAY_ANIMATION, OnPlayAnimation)
    ON_UPDATE_COMMAND_UI(ID_PLAY_ANIMATION, OnUpdatePlayAnimation)
    ON_COMMAND(ID_STOP_ANIMATION, OnStopAnimation)
    ON_UPDATE_COMMAND_UI(ID_STOP_ANIMATION, OnUpdateStopAnimation)
    ON_COMMAND(ID_TOGGLE_ROTATION, OnToggleRotation)
    ON_UPDATE_COMMAND_UI(ID_TOGGLE_ROTATION, OnUpdateToggleRotation)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

ContentPreviewPane::ContentPreviewPane() :
    _currentFrame(0),
    _animationTick(0),
    _toolbarVisible(false),
    _prevToolbarVisible(true),
    _playAnimation(false),
    _rotate(false)
{

}

ContentPreviewPane::~ContentPreviewPane()
{

}

_Use_decl_annotations_
void ContentPreviewPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
    UNREFERENCED_PARAMETER(pWnd);
    UNREFERENCED_PARAMETER(point);
    return;
}

_Use_decl_annotations_
BOOL ContentPreviewPane::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return FALSE;
}

void ContentPreviewPane::OnPlayAnimation()
{
    _playAnimation = true;
}

_Use_decl_annotations_
void ContentPreviewPane::OnUpdatePlayAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_playAnimation);
}

void ContentPreviewPane::OnStopAnimation()
{
    _playAnimation = false;
}

void ContentPreviewPane::OnToggleRotation()
{
    _rotate = !_rotate;
}

_Use_decl_annotations_
void ContentPreviewPane::OnUpdateToggleRotation(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(_rotate);
}

_Use_decl_annotations_
void ContentPreviewPane::OnUpdateStopAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_playAnimation);
}

_Use_decl_annotations_
void ContentPreviewPane::OnSize(UINT nType, int cx, int cy) 
{
    _prevToolbarVisible = !_prevToolbarVisible;
    RenderingPane::OnSize(nType, cx, cy);
}

_Use_decl_annotations_
int ContentPreviewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (RenderingPane::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    if (!_toolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER_PREVIEW_PANE_TOOLBAR))
    {
        return -1;
    }

    _toolBar.LoadToolBar(IDR_EXPLORER_PREVIEW_PANE_TOOLBAR, 0, 0, TRUE /* Is locked */);
    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    _toolBar.SetPaneStyle(_toolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
    _toolBar.SetOwner(this);

    CRect comboBoxRect;
    comboBoxRect.SetRectEmpty();
    int nIndex = _toolBar.CommandToIndex(IDC_ANIMATIONFRAMES_COMBOBOX);
    _toolBar.GetItemRect(nIndex, &comboBoxRect);

    // comboBoxRect is being returned as 0,0,0,0 for some reason.
    // Ensure that it is properly placed on the toolbar by forcing
    // a valid size.
    comboBoxRect.top = 0;
    comboBoxRect.bottom = comboBoxRect.top + 100 /*drop height*/;
    comboBoxRect.left = 64; // 3 x toolbar button size
    comboBoxRect.right = comboBoxRect.left + 80;

    // Create combo:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (!_animationFramesComboBox.Create(dwViewStyle, comboBoxRect, &_toolBar, IDC_ANIMATIONFRAMES_COMBOBOX))
    {
        return -1;
    }

    SetControlsFont();

    // All commands will be routed via this control , not via the parent frame:
    _toolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();

    return 0;
}

void ContentPreviewPane::OnAdjustLayout()
{
    if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    if (_toolbarVisible != _prevToolbarVisible)
    {
        if (_toolbarVisible)
        {
            // Set the toolbar position
            int cyTlb = _toolBar.CalcFixedLayout(FALSE, TRUE).cy;
            _toolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
            // Set the render window position below the toolbar
            _renderWindow.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
        }
        else
        {
            _toolBar.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER);
            _renderWindow.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }

    _prevToolbarVisible = _toolbarVisible;
}

_Use_decl_annotations_
HRESULT ContentPreviewPane::SetTextureContent(const std::shared_ptr<GDK::TextureContent>& content) 
{ 
    _currentFrame = 0;
    _geometry = nullptr;
    _sphere.reset();

    if (_graphics && content)
    {
        _texture = _graphics->CreateTexture(content);
    }

    if (_toolbarVisible)
    {
        _toolbarVisible = false;
        AdjustLayout();
    }

    return S_OK;
};

_Use_decl_annotations_
void ContentPreviewPane::CollectTextures(const ContentPaneTreeData& content, std::vector<std::wstring>& textures)
{
    // Handle special geometry that has encoded % texture names.
    if (std::IsEncodedTextureContentPath(content.contentPath))
    {
        textures.push_back(std::GetTextureContentPathFromGeometryContentPath(content.contentPath));
        return;
    }

    std::wstring contentMapping = content.contentMapping;
    std::wstring contentMappingFullPath = content.contentMappingRoot;
    std::wstring root = content.rootFilePath;

    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    std::wstring actualSearch = root + L"\\*.texture";

    hFind = FindFirstFile(actualSearch.c_str(), &FindData);
    do
    {
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return;
        }

        std::wstring fileName (FindData.cFileName);

        if (fileName == L"." || fileName == L"..")
        {
            continue;
        }

        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            continue;
        }

        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }
        else
        {
            // Add file
            ContentPaneTreeData data;
            data.rootFilePath = root;
            data.filePath = root + L"\\" + fileName;
            data.fileBasedContent = true;
            data.isDirectory = false;
            data.expandedOnce = false;
            data.contentMapping = contentMapping;
            data.contentPath = data.filePath.substr(contentMappingFullPath.length() + 1);
            data.contentMappingRoot = contentMappingFullPath;

            textures.push_back(data.contentPath);
        }
    } while (FindNextFile(hFind, &FindData));

    FindClose(hFind);
}

_Use_decl_annotations_
HRESULT ContentPreviewPane::SetGeometryContent(const ContentPaneTreeData& content)
{
    std::vector<std::wstring> textures;
    CollectTextures(content, textures);
    if (textures.size() > 0)
    {
        return SetObjectContent(GDK::Content::LoadGeometryContent(content.contentPath), GDK::Content::LoadTextureContent(textures[0]));
    }

    // If no textures were found, just set the geometry content
    return SetGeometryContent(GDK::Content::LoadGeometryContent(content.contentPath));
}

_Use_decl_annotations_
HRESULT ContentPreviewPane::SetGeometryContent(const std::shared_ptr<GDK::GeometryContent>& content) 
{
    HRESULT hr = S_OK;
    _texture = nullptr;

    CollectAnimationFrames(content);

    if (!_toolbarVisible)
    {
        _toolbarVisible = true;
        AdjustLayout();
    }

    if (_animationFrames.size())
    {
        _currentFrame = _animationFrames[_animationFramesComboBox.GetCurSel()].startFrame;
    }

    if (_graphics && content)
    {
        _geometry = _graphics->CreateGeometry(content);
        _sphere = GDK::Collision::SphereFromGeometry(content, 0);

        GDK::Sphere* sphere = static_cast<GDK::Sphere*>(_sphere.get());
        float distFromObject = (_camera->GetAspect() * sphere->radius) / tanf(_camera->GetFOV() * 0.5f);
        _camera->SetViewParams(sphere->center + GDK::Vector3::Forward() * distFromObject, sphere->center, GDK::Vector3::Up());
    }

    return hr;
};

_Use_decl_annotations_
void ContentPreviewPane::CollectAnimationFrames(std::shared_ptr<GDK::GeometryContent> content)
{
    _animationFrames.clear();
    if (_animationFramesComboBox)
    {
        _animationFramesComboBox.ResetContent();
    }

    if (!content)
    {
        return;
    }

    uint32_t numAnimations = content->GetNumAnimations();
    const GDK::GeometryContent::Animation* animations = content->GetAnimations();
    for (uint32_t a = 0; a < numAnimations; a++)
    {
        AnimationFrameInfo aniInfo;
        aniInfo.name = std::AnsiToWide(animations[a].Name);
        aniInfo.startFrame = animations[a].StartFrame;
        aniInfo.endFrame = animations[a].EndFrame;
        _animationFrames.push_back(aniInfo);
        _animationFramesComboBox.AddString(aniInfo.name.c_str());
    }

    _animationFramesComboBox.SetCurSel(0);
}

_Use_decl_annotations_
HRESULT ContentPreviewPane::SetObjectContent(const std::shared_ptr<GDK::GeometryContent>& geometryContent, const std::shared_ptr<GDK::TextureContent>& textureContent)
{
    _currentFrame = 0;

    HRESULT hr = S_OK;

    CollectAnimationFrames(geometryContent);

    if (!_toolbarVisible)
    {
        _toolbarVisible = true;
        AdjustLayout();
    }

    if (_graphics && textureContent && geometryContent)
    {
        _texture = _graphics->CreateTexture(textureContent);
        _geometry = _graphics->CreateGeometry(geometryContent);
        _sphere = GDK::Collision::SphereFromGeometry(geometryContent, 0);

        GDK::Sphere* sphere = static_cast<GDK::Sphere*>(_sphere.get());
        float distFromObject = (_camera->GetAspect() * sphere->radius) / tanf(_camera->GetFOV() * 0.5f);
        _camera->SetViewParams(sphere->center + GDK::Vector3::Forward() * distFromObject, sphere->center, GDK::Vector3::Up());
    }

    return hr;
}

void ContentPreviewPane::OnRender()
{
    if (_graphics)
    {
        _graphics->BindTexture(0, _texture);
        _graphics->BindGeometry(_geometry);

        if (_geometry)
        {
            _animationTick += theApp.GetElapsedGameTime();
            if (_animationTick > .1f && (_playAnimation))
            {
                _currentFrame++;
                if (_currentFrame > _animationFrames[_animationFramesComboBox.GetCurSel()].endFrame)
                {
                    _currentFrame = _animationFrames[_animationFramesComboBox.GetCurSel()].startFrame;
                }

                _animationTick = 0;
            }
            _graphics->Draw(_rotate ? GDK::Matrix::CreateRotationY(theApp.GetTotalElapsedGameTime() * 0.4f) : GDK::Matrix::Identity(), _currentFrame);
        }
        else if (_texture)
        {
            uint32_t widthHeight = max(_texture->GetWidth(), _texture->GetHeight());
            _graphics->Set2DWorkArea(widthHeight,widthHeight);
            _graphics->Draw2D(_texture,0,0);
        }
    }
}

void ContentPreviewPane::OnGraphicsDeviceChanged()
{
    _geometry = nullptr;
    _texture = nullptr;
    _sphere.reset();
}

void ContentPreviewPane::SetControlsFont()
{
    ::DeleteObject(_font.Detach());

    LOGFONT lf;
    afxGlobalData.fontRegular.GetLogFont(&lf);

    NONCLIENTMETRICS info;
    info.cbSize = sizeof(info);

    afxGlobalData.GetNonClientMetrics(info);

    lf.lfHeight = info.lfMenuFont.lfHeight;
    lf.lfWeight = info.lfMenuFont.lfWeight;
    lf.lfItalic = info.lfMenuFont.lfItalic;

    _font.CreateFontIndirect(&lf);

    _animationFramesComboBox.SetFont(&_font);
}

void ContentPreviewPane::OnSelchangeCombo1()
{
    int currentSelection = _animationFramesComboBox.GetCurSel();
    CString strValue;
    _animationFramesComboBox.GetLBText(currentSelection, strValue);
    for (size_t i = 0; i < _animationFrames.size(); i++)
    {
        if (strValue.CompareNoCase(_animationFrames[i].name.c_str()) == 0)
        {
            _currentFrame = _animationFrames[i].startFrame;
            break;
        }
    }
}
