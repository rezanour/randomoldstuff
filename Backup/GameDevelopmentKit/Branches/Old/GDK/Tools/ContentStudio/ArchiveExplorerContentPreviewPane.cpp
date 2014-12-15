#include "stdafx.h"
#include "ArchiveExplorerContentPreviewPane.h"

BEGIN_MESSAGE_MAP(ArchiveExplorerContentPreviewPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_ANIMATIONFRAMES_COMBOBOX, &ArchiveExplorerContentPreviewPane::OnSelchangeCombo1)
    ON_COMMAND(ID_PLAY_ANIMATION, OnPlayAnimation)
    ON_UPDATE_COMMAND_UI(ID_PLAY_ANIMATION, OnUpdatePlayAnimation)
    ON_COMMAND(ID_STOP_ANIMATION, OnStopAnimation)
    ON_UPDATE_COMMAND_UI(ID_STOP_ANIMATION, OnUpdateStopAnimation)
END_MESSAGE_MAP()

ArchiveExplorerContentPreviewPane::ArchiveExplorerContentPreviewPane() :
    _currentFrame(0),
    _animationTick(0),
    _toolbarVisible(false),
    _playAnimation(false)
{

}

ArchiveExplorerContentPreviewPane::~ArchiveExplorerContentPreviewPane()
{

}

void ArchiveExplorerContentPreviewPane::OnPlayAnimation()
{
    _playAnimation = true;
}

void ArchiveExplorerContentPreviewPane::OnUpdatePlayAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_playAnimation);
}

void ArchiveExplorerContentPreviewPane::OnStopAnimation()
{
    _playAnimation = false;
}

void ArchiveExplorerContentPreviewPane::OnUpdateStopAnimation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_playAnimation);
}

void ArchiveExplorerContentPreviewPane::OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy) 
{
    RenderingPane::OnSize(nType, cx, cy);
}

int ArchiveExplorerContentPreviewPane::OnCreate(_In_ LPCREATESTRUCT lpCreateStruct)
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

void ArchiveExplorerContentPreviewPane::OnAdjustLayout()
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

HRESULT ArchiveExplorerContentPreviewPane::SetTextureResource(_In_ GDK::Content::ITextureResource* resource) 
{ 
    _textureResource = resource;
    _currentFrame = 0;

    if (_graphics)
    {
        return _graphics->CreateRuntimeTexture(resource, _textureRuntimeResource.ReleaseAndGetAddressOf());
    }

    return S_OK;
};

HRESULT ArchiveExplorerContentPreviewPane::SetGeometryResource(_In_ GDK::Content::IGeometryResource* resource) 
{
    HRESULT hr = S_OK;
    _geometryResource = resource; 

    CollectAnimationFrames(_geometryResource.Get());
    AdjustLayout();

    if (_animationFrames.size())
    {
        _currentFrame = _animationFrames[_animationFramesComboBox.GetCurSel()].startFrame;
    }

    if (_graphics)
    {
        hr = _graphics->CreateRuntimeGeometry(resource, _geometryRuntimeResource.ReleaseAndGetAddressOf());
        
        if (_geometryRuntimeResource)
        {
            _geometryRuntimeResource->SetCurrentFrame(_currentFrame);
        }
    }

    return hr;
};

void ArchiveExplorerContentPreviewPane::CollectAnimationFrames(_In_ GDK::Content::IGeometryResource* gresource)
{
    _animationFrames.clear();
    if (_animationFramesComboBox)
    {
        _animationFramesComboBox.ResetContent();
    }

    if (!gresource)
    {
        _toolbarVisible = false;
        return;
    }

    _toolbarVisible = true;

    uint32_t numFrames = 0;
    gresource->GetFrameCount(&numFrames);
    for (uint32_t f = 0; f < numFrames; f++)
    {
        const wchar_t* frameName = nullptr;
        std::wstring theframeName;
        bool theframeNameComplete = false;

        gresource->GetFrameName(f, &frameName);
        for(int i = 0; i < lstrlenW(frameName); i++)
        {
            if (isdigit(frameName[i]))
            {
                // Ensure that we capture the first digit of the name
                // because it marks the frame set.
                if (!theframeNameComplete)
                {
                    theframeName += frameName[i];
                    break;
                }
            }
            else
            {
                theframeName += frameName[i];
            }
        }

        AddAnimationFrame(theframeName,f);
    }

    if (_animationFramesComboBox)
    {
        // Populate animation frames combobox
        for (size_t i = 0; i < _animationFrames.size(); i++)
        {
            _animationFramesComboBox.AddString(_animationFrames[i].name.c_str());
        }
        _animationFramesComboBox.SetCurSel(0);
    }
}

void ArchiveExplorerContentPreviewPane::AddAnimationFrame(_In_ std::wstring name, _In_ size_t frameIndex)
{
    bool foundFrame = false;
    for (size_t i = 0; i < _animationFrames.size(); i++)
    {
        if (_animationFrames[i].name == name)
        {
            _animationFrames[i].endFrame = frameIndex;
            foundFrame = true;
            break;
        }
    }

    if (!foundFrame)
    {
        AnimationFrameInfo newFrame;
        newFrame.name = name;
        newFrame.startFrame = frameIndex;
        newFrame.endFrame = frameIndex;
        _animationFrames.push_back(newFrame);
    }
}

HRESULT ArchiveExplorerContentPreviewPane::SetObjectResources(_In_ GDK::Content::IGeometryResource* gresource, _In_ GDK::Content::ITextureResource* tresource)
{
    _currentFrame = 0;
    _geometryResource = gresource;
    _textureResource = tresource;

    HRESULT hr = S_OK;

    CollectAnimationFrames(_geometryResource.Get());
    AdjustLayout();

    if (_graphics)
    {
        hr = _graphics->CreateRuntimeGeometry(gresource, _geometryRuntimeResource.ReleaseAndGetAddressOf());
        if (SUCCEEDED(hr))
        {
            hr = _graphics->CreateRuntimeTexture(tresource, _textureRuntimeResource.ReleaseAndGetAddressOf());
        }

        if (_geometryRuntimeResource)
        {
            _geometryRuntimeResource->SetCurrentFrame(_currentFrame);
        }
    }

    return hr;
}

HRESULT ArchiveExplorerContentPreviewPane::UpdateCurrentFrame(_In_ bool incrementFrame)
{
    if (_geometryRuntimeResource)
    {
        size_t count = _geometryRuntimeResource->GetNumFrames();
        if (incrementFrame)
        {
            _currentFrame = (_currentFrame + 1 < count) ? _currentFrame + 1 : 0;
        }
        else
        {
            _currentFrame = (_currentFrame > 0) ? _currentFrame - 1 : count - 1;
        }
        _geometryRuntimeResource->SetCurrentFrame(_currentFrame);
    }

    return S_OK;
}

void ArchiveExplorerContentPreviewPane::OnRender()
{
    if (_graphics)
    {
        _graphics->SetFillMode(_fillMode);
        _graphics->BindTexture(0, _textureRuntimeResource.Get());
        _graphics->BindGeometry(_geometryRuntimeResource.Get());

        if (_geometryRuntimeResource)
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
                _geometryRuntimeResource->SetCurrentFrame(_currentFrame);
            }

            // This is a preview pane, so always draw the content at the world origin
            _graphics->Draw(DirectX::XMMatrixRotationY(theApp.GetTotalElapsedGameTime() * 0.4f));
        }
        else if (_textureRuntimeResource)
        {
            RECT rect = { 10, 10, 90, 90 }; // percentages
            _graphics->Draw2D(rect);
        }
    }
}

void ArchiveExplorerContentPreviewPane::OnGraphicsDeviceChanged()
{
    _geometryRuntimeResource = nullptr;
    _textureRuntimeResource = nullptr;
}

void ArchiveExplorerContentPreviewPane::SetControlsFont()
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

void ArchiveExplorerContentPreviewPane::OnSelchangeCombo1()
{
    int currentSelection = _animationFramesComboBox.GetCurSel();
    CString strValue;
    _animationFramesComboBox.GetLBText(currentSelection, strValue);
    for (size_t i = 0; i < _animationFrames.size(); i++)
    {
        if (strValue.CompareNoCase(_animationFrames[i].name.c_str()) == 0)
        {
            _currentFrame = _animationFrames[i].startFrame;
            if (_geometryRuntimeResource)
            {
                _geometryRuntimeResource->SetCurrentFrame(_currentFrame);
            }
            break;
        }
    }
}
