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
    void OnContentSelectionChanged();
    void OnContentSelectionOpened();
    void OnContentFrameChanged(_In_ bool incrementFrame);

public:
    ContentStudioDoc* GetDocument() const;
    MainFrame* GetMainFrame() const;

    void OnRenderCallback();
    virtual void OnDraw(CDC* pDC);
    virtual BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual ~ContentStudioView();

private:
    MainFrame* _mainFrame;
    size_t _indexToGraphicsComponent;
    size_t _indexToGamesComponent;
    bool _dropTargetRegistered;

    std::shared_ptr<GDK::GraphicsDevice> _graphics;
    std::shared_ptr<GDK::Game> _game;

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
    std::wstring GetDroppedContent(COleDataObject* pDataObject);

    bool IsSupportedDroppedContent(COleDataObject* pDataObject);

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
