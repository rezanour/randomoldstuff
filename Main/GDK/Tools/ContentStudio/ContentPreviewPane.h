#pragma once

class ContentPreviewPaneToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(_In_ CFrameWnd* /*pTarget*/, _In_ BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class ContentPreviewPane : public RenderingPane
{
public:
    ContentPreviewPane();

    HRESULT SetTextureContent(_In_ const std::shared_ptr<GDK::TextureContent>& content);
    HRESULT SetGeometryContent(_In_ const std::shared_ptr<GDK::GeometryContent>& content);
    HRESULT SetGeometryContent(_In_ const ContentPaneTreeData& content);
    HRESULT SetObjectContent(_In_ const std::shared_ptr<GDK::GeometryContent>& graphicsContent, _In_ const std::shared_ptr<GDK::TextureContent>& textureContent);

public:
    virtual ~ContentPreviewPane();
    void OnRender();
    void OnGraphicsDeviceChanged();
    void OnAdjustLayout();
    void OnPlayAnimation();
    void OnUpdatePlayAnimation(_In_ CCmdUI* pCmdUI);
    void OnStopAnimation();
    void OnUpdateStopAnimation(_In_ CCmdUI* pCmdUI);
    void OnToggleRotation();
    void OnUpdateToggleRotation(_In_ CCmdUI* pCmdUI);
    afx_msg void OnSelchangeCombo1();
    virtual BOOL OnEraseBkgnd(_In_ CDC* pDC);

private:
    void SetControlsFont();
    void CollectAnimationFrames(_In_ std::shared_ptr<GDK::GeometryContent> gresource);
    void CollectTextures(_In_ const ContentPaneTreeData& content, _Inout_ std::vector<std::wstring>& textures);

    struct AnimationFrameInfo
    {
        uint32_t startFrame;
        uint32_t endFrame;
        std::wstring name;
    };

    std::shared_ptr<GDK::Texture> _texture;
    std::shared_ptr<GDK::Geometry> _geometry;
    std::shared_ptr<GDK::CollisionPrimitive> _sphere;

    uint32_t _currentFrame;
    std::vector<AnimationFrameInfo> _animationFrames;
    float _animationTick;
    bool _rotate;

    ContentPreviewPaneToolBar _toolBar;
    CFont _font;
    CComboBox _animationFramesComboBox;
    bool _toolbarVisible;
    bool _prevToolbarVisible;
    bool _playAnimation;
    afx_msg int OnCreate(_In_ LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(_In_ UINT nType, _In_ int cx, _In_ int cy);
    afx_msg void OnContextMenu(_In_ CWnd* pWnd, _In_ CPoint point);

    DECLARE_MESSAGE_MAP()
};
