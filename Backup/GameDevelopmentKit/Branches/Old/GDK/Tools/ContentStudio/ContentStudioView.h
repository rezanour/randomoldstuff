#pragma once

#define MAX_RENDERERS 10
#define MAX_GAMES 10
#define INVALID_SELECTION_INDEX (size_t)-1

class ContentStudioView : public CView, public IGameHost, public INotifyView
{
protected:
    ContentStudioView();
    DECLARE_DYNCREATE(ContentStudioView)

    // IUnknown
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // IGameHost
public:

    // INotifyView
public:
    void OnDocumentChanged();
    void OnContentSelectionChanged();
    void OnContentSelectionOpened();
    void OnContentFrameChanged(_In_ bool incrementFrame);

public:
    ContentStudioDoc* GetDocument() const;
    MainFrame* GetMainFrame() const;

    void OnRenderCallback();
    void OnRenderOverlayCallback(bool erase);
    void OnDrawOverlays(CDC* pDC);
    virtual void OnDraw(CDC* pDC);
    virtual BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual ~ContentStudioView();

private:
    MainFrame* _mainFrame;
    size_t _indexToGraphicsComponent;
    size_t _indexToGamesComponent;
    size_t _indexToContentComponent;

    CWnd _transparentWindow;
    ComPtr<IGraphicsDevice> _graphics;

    ComPtr<IGameFactory> _gameFactory;
    ComPtr<IGame> _game;
    ComPtr<IGameEdit> _gameEdit;
    ComPtr<IWorldEdit> _worldEdit;
    ComPtr<IGameObjectEdit> _gameObjectEdit;

    Camera _archiveContentPreviewCamera;
    Camera _worldCamera;

    COleDropTarget _oleDropTarget;

private:
    RenderingMode GetCurrentRenderingMode();
    
    void DrawGradientBackground(CDC* pDC);

    void SwitchGraphicsDevice(size_t index);
    void SwitchGame(size_t index);

    void PopulateArchiveExplorer();

    void SetViewNotificationHandlers(INotifyView* notifyView);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNcDestroy();
    afx_msg void OnSettingsGraphics();
    afx_msg void OnUpdateSettingsGraphics(CCmdUI *pCmdUI);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnGraphics(UINT id);
    afx_msg void OnUpdateGraphics(CCmdUI *pCmdUI);
    afx_msg void OnGames(UINT id);
    afx_msg void OnUpdateGames(CCmdUI *pCmdUI);
    afx_msg void OnTestrendermodesWireframerendering();
    afx_msg void OnTestrendermodesSolidrendering();
    afx_msg void OnUpdateTestrendermodesWireframerendering(CCmdUI *pCmdUI);
    afx_msg void OnUpdateTestrendermodesSolidrendering(CCmdUI *pCmdUI);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

#ifndef _DEBUG
inline ContentStudioDoc* ContentStudioView::GetDocument() const
   { return reinterpret_cast<ContentStudioDoc*>(m_pDocument); }

inline MainFrame* ContentStudioView::GetMainFrame() const
   { return reinterpret_cast<MainFrame*>(_mainFrame); }

#endif
