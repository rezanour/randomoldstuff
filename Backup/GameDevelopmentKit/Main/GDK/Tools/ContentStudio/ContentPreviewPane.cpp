#include "stdafx.h"
#include "ContentPreviewPane.h"

BEGIN_MESSAGE_MAP(ContentPreviewPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_ANIMATIONFRAMES_COMBOBOX, &ContentPreviewPane::OnSelchangeCombo1)
    ON_COMMAND(ID_PLAY_ANIMATION, OnPlayAnimation)
    ON_UPDATE_COMMAND_UI(ID_PLAY_ANIMATION, OnUpdatePlayAnimation)
    ON_COMMAND(ID_STOP_ANIMATION, OnStopAnimation)
    ON_UPDATE_COMMAND_UI(ID_STOP_ANIMATION, OnUpdateStopAnimation)
END_MESSAGE_MAP()

ContentPreviewPane::ContentPreviewPane() :
    _currentFrame(0),
    _animationTick(0),
    _toolbarVisible(false),
    _playAnimation(false)
{

}

ContentPreviewPane::~ContentPreviewPane()
{

}

void ContentPreviewPane::OnPlayAnimation()
{
    _playAnimation = true;
}

void ContentPreviewPane::OnUpdatePlayAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_playAnimation);
}

void ContentPreviewPane::OnStopAnimation()
{
    _playAnimation = false;
}

void ContentPreviewPane::OnUpdateStopAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_playAnimation);
}

void ContentPreviewPane::OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy) 
{
    RenderingPane::OnSize(nType, cx, cy);
}

int ContentPreviewPane::OnCreate(_In_ LPCREATESTRUCT lpCreateStruct)
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
    comboBoxRect.left = 48; // 2 x toolbar button size
    comboBoxRect.right = comboBoxRect.left + 80;

    // Create combo:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

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

    if (_toolbarVisible)
    {
        CRect rectClient;
        GetClientRect(rectClient);

        // Set the toolbar position
        int cyTlb = _toolBar.CalcFixedLayout(FALSE, TRUE).cy;
        _toolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
        // Set the render window position below the toolbar
        _renderWindow.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    }
}

HRESULT ContentPreviewPane::SetTextureContent(_In_ const std::shared_ptr<GDK::TextureContent>& content) 
{ 
    _textureContent = content;
    _currentFrame = 0;

    if (_graphics && content)
    {
        _texture = _graphics->CreateTexture(content);
    }

    return S_OK;
};

HRESULT ContentPreviewPane::SetGeometryContent(_In_ const std::shared_ptr<GDK::GeometryContent>& content) 
{
    HRESULT hr = S_OK;
    _geometryContent = content; 

    CollectAnimationFrames(_geometryContent);
    AdjustLayout();

    if (_animationFrames.size())
    {
        _currentFrame = _animationFrames[_animationFramesComboBox.GetCurSel()].startFrame;
    }

    if (_graphics && content)
    {
        _geometry = _graphics->CreateGeometry(content);
        if (_geometry)
        {
            _geometry->Draw(_currentFrame);
        }
    }

    return hr;
};

void ContentPreviewPane::CollectAnimationFrames(_In_ std::shared_ptr<GDK::GeometryContent> content)
{
    _animationFrames.clear();
    if (_animationFramesComboBox)
    {
        _animationFramesComboBox.ResetContent();
    }

    if (!content)
    {
        _toolbarVisible = false;
        return;
    }

    _toolbarVisible = true;

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

HRESULT ContentPreviewPane::SetObjectContent(_In_ const std::shared_ptr<GDK::GeometryContent>& geometryContent, _In_ const std::shared_ptr<GDK::TextureContent>& textureContent)
{
    _currentFrame = 0;
    _geometryContent = geometryContent;
    _textureContent = textureContent;

    HRESULT hr = S_OK;

    CollectAnimationFrames(_geometryContent);
    AdjustLayout();

    if (_graphics && textureContent && geometryContent)
    {
        _texture = _graphics->CreateTexture(textureContent);
        _geometry = _graphics->CreateGeometry(geometryContent);

        if (_geometry)
        {
            _geometry->Draw(_currentFrame);
        }
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
                _geometry->Draw(_currentFrame);
            }
            _graphics->Draw(DirectX::XMMatrixRotationY(theApp.GetTotalElapsedGameTime() * 0.4f));
        }
        else
        {
            RectangleF rect(10.0f, 10.0f, 90.0f, 90.0f); // percentages
            _graphics->Draw2D(rect);
        }
    }
}

void ContentPreviewPane::OnGraphicsDeviceChanged()
{
    _geometry = nullptr;
    _texture = nullptr;
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
            if (_geometry)
            {
                _geometry->Draw(_currentFrame);
            }
            break;
        }
    }
}
