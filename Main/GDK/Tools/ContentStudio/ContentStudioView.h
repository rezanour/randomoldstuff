#pragma once

#define MAX_RENDERERS 10
#define MAX_GAMES 10
#define INVALID_SELECTION_INDEX (size_t)-1

class ContentStudioView : public CView, public INotifyView
{
protected:
    ContentStudioView();
    DECLARE_DYNCREATE(ContentStudioView)

    // INotifyView
public:
    void OnDocumentChanged();
    void OnContentSelectionChanged(_In_ const ContentPaneTreeData& selectedContent);
    void OnContentExpanded(_In_ const ContentPaneTreeData& selectedContent);
    void OnContentSelectionOpened();
    void OnCreateObject(_In_ const std::wstring& name);

public:
    ContentStudioDoc* GetDocument() const;
    MainFrame* GetMainFrame() const;

    void OnRenderCallback();
    virtual void OnDraw(_In_ CDC* pDC);
    virtual BOOL OnEraseBkgnd(_In_ CDC* pDC);
    virtual BOOL PreCreateWindow(_In_ CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual ~ContentStudioView();

private:
    MainFrame* _mainFrame;
    size_t _indexToGraphicsComponent;
    size_t _indexToGamesComponent;
    bool _dropTargetRegistered;
    std::shared_ptr<GDK::GraphicsDevice> _graphics;
    std::shared_ptr<GDK::AudioDevice> _audio;
    std::shared_ptr<GDK::IRunningGame> _game;
    std::shared_ptr<GDK::IGameObjectEdit> _selectedGameObject;
    std::shared_ptr<GDK::IGameObjectEdit> _copiedGameObject;
    CPoint _dragEnterPoint;
    void ResetContentPreviewCamera();
    void ResetWorldCamera();

    FirstPersonCameraWithInput _contentPreviewPaneCamera;
    FirstPersonCameraWithInput _worldCamera;

    std::shared_ptr<COleDropTarget> _oleDropTarget;

    bool _processAddContent;
    size_t _contentListCount;

    GDK::Vector2 _startMousePoint;
    bool _draggingObjectFromContentPane;

    std::shared_ptr<Gizmo> _gizmo;

private:
    RenderingMode GetCurrentRenderingMode();
    
    void DrawGradientBackground(_In_ CDC* pDC);

    void SwitchGraphicsDevice(_In_ GDK::GraphicsDevice::Type type);
    void SwitchAudioDevice(_In_ GDK::AudioDevice::Type type);
    void SwitchGame(_In_ size_t index);

    void PopulateContentPane(_In_ bool clearAll);

    void SetViewNotificationHandlers(_In_ INotifyView* notifyView);
    std::wstring GetDroppedContent(_In_ COleDataObject* pDataObject);
    bool IsSupportedDroppedContent(_In_ COleDataObject* pDataObject);
    GDK::GraphicsDevice::Type _graphicsType;
    GDK::AudioDevice::Type _audioType;

    void UpdateViewInput();

    bool GetScreenRaycast(_In_ const CPoint& point, _Out_ GDK::Ray* ray);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(_In_ CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(_In_ UINT nFlags, _In_ CPoint point);
    afx_msg void OnContextMenu(_In_ CWnd* pWnd, _In_ CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNcDestroy();
    afx_msg void OnSettingsGraphics();
    afx_msg void OnUpdateSettingsGraphics(_In_ CCmdUI *pCmdUI);
    afx_msg void OnInitMenuPopup(_In_ CMenu* pPopupMenu, _In_ UINT nIndex, _In_ BOOL bSysMenu);
    afx_msg void OnGraphics(_In_ UINT id);
    afx_msg void OnUpdateGraphics(_In_ CCmdUI *pCmdUI);
    afx_msg void OnAudio(_In_ UINT id);
    afx_msg void OnUpdateAudio(_In_ CCmdUI *pCmdUI);
    afx_msg void OnGames(_In_ UINT id);
    afx_msg void OnUpdateGames(_In_ CCmdUI *pCmdUI);
    afx_msg void OnTestrendermodesWireframerendering();
    afx_msg void OnTestrendermodesSolidrendering();
    afx_msg void OnUpdateTestrendermodesWireframerendering(_In_ CCmdUI *pCmdUI);
    afx_msg void OnUpdateTestrendermodesSolidrendering(_In_ CCmdUI *pCmdUI);
    virtual DROPEFFECT OnDragOver(_In_ COleDataObject* pDataObject, _In_ DWORD dwKeyState, _In_ CPoint point);
    virtual DROPEFFECT OnDragEnter(_In_ COleDataObject* pDataObject, _In_ DWORD dwKeyState, _In_ CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(_In_ COleDataObject* pDataObject, _In_ DROPEFFECT dropEffect, _In_ CPoint point);
    afx_msg BOOL OnSetCursor(_In_ CWnd* pWnd, _In_ UINT nHitTest, _In_ UINT message);
    afx_msg void OnSaveWorld();
    afx_msg void OnNewWorld();
    afx_msg void OnSaveWorldAs();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnEditCut();
    afx_msg void OnClose();
    virtual LRESULT DefWindowProc(_In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnWorldRename();
};

#ifndef _DEBUG
inline ContentStudioDoc* ContentStudioView::GetDocument() const
   { return reinterpret_cast<ContentStudioDoc*>(m_pDocument); }

inline MainFrame* ContentStudioView::GetMainFrame() const
   { return reinterpret_cast<MainFrame*>(_mainFrame); }

#endif
