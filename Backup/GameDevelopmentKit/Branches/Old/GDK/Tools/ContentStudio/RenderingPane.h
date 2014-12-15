#pragma once

class RenderingPane : public CDockablePane
{
public:
    RenderingPane();

public:
    virtual ~RenderingPane();
    void SetGraphicsDevice(IGraphicsDevice* graphicsDevice);
    HWND GetRenderWindow() { return _renderWindow.m_hWnd; }
    IGraphicsDevice* GetGraphicsDevice() { return _graphics.Get(); }
    void SetCamera(_In_ Camera* camera) { _camera = camera; };
    void Render();
    void SetFillMode(_In_ GDK::Graphics::GraphicsFillMode fillMode) { _fillMode = fillMode; }

    virtual void OnRender() { }
    virtual void OnGraphicsDeviceChanged() { }
    virtual void OnAdjustLayout() { }

protected:
    CWnd _renderWindow;
    ComPtr<IGraphicsDevice> _graphics;
    Camera* _camera;
    GDK::Graphics::GraphicsFillMode _fillMode;

protected:
    void AdjustLayout();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
};
