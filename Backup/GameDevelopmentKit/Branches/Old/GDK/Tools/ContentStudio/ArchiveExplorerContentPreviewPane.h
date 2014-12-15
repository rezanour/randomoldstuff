#pragma once

class ArchiveExplorerContentPreviewPaneToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(_In_ CFrameWnd* /*pTarget*/, _In_ BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class ArchiveExplorerContentPreviewPane : public RenderingPane
{
public:
    ArchiveExplorerContentPreviewPane();

    HRESULT SetTextureResource(_In_ GDK::Content::ITextureResource* resource);
    HRESULT SetGeometryResource(_In_ GDK::Content::IGeometryResource* resource);
    HRESULT SetObjectResources(_In_ GDK::Content::IGeometryResource* gresource, _In_ GDK::Content::ITextureResource* tresource);
    HRESULT UpdateCurrentFrame(_In_ bool incrementFrame);

public:
    virtual ~ArchiveExplorerContentPreviewPane();
    void OnRender();
    void OnGraphicsDeviceChanged();
    void OnAdjustLayout();
    void OnPlayAnimation();
    void OnUpdatePlayAnimation(CCmdUI* pCmdUI);
    void OnStopAnimation();
    void OnUpdateStopAnimation(CCmdUI* pCmdUI);
    afx_msg void OnSelchangeCombo1();

private:
    void SetControlsFont();
    void CollectAnimationFrames(_In_ GDK::Content::IGeometryResource* gresource);
    void AddAnimationFrame(_In_ std::wstring name, _In_ size_t frameIndex);

    struct AnimationFrameInfo
    {
        size_t startFrame;
        size_t endFrame;
        std::wstring name;
    };

    ComPtr<GDK::Content::ITextureResource> _textureResource;
    ComPtr<GDK::Graphics::IRuntimeTexture> _textureRuntimeResource;

    ComPtr<GDK::Content::IGeometryResource> _geometryResource;
    ComPtr<GDK::Graphics::IRuntimeGeometry> _geometryRuntimeResource;

    size_t _currentFrame;
    std::vector<AnimationFrameInfo> _animationFrames;
    float _animationTick;

    ArchiveExplorerContentPreviewPaneToolBar _toolBar;
    CFont _font;
    CComboBox _animationFramesComboBox;
    bool _toolbarVisible;
    bool _playAnimation;
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
};
