#include "stdafx.h"
#include "RenderingPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

RenderingPane::RenderingPane() :
    _camera(nullptr)
{

}

RenderingPane::~RenderingPane()
{
}

BEGIN_MESSAGE_MAP(RenderingPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

_Use_decl_annotations_
void RenderingPane::SetGraphicsDevice(std::shared_ptr<GDK::GraphicsDevice> graphicsDevice) 
{ 
    OnGraphicsDeviceChanged(); 
    _graphics = graphicsDevice; 
}

_Use_decl_annotations_
int RenderingPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create rendering window
    if (!_renderWindow.CreateEx(0, _T("STATIC"), NULL, WS_CHILD | WS_VISIBLE, -1, -1, 0, 0, m_hWnd, NULL))
    {
        return -1;
    }

    return 0;
}

void RenderingPane::AdjustLayout()
{
    if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
    {
        return;
    }

    // Resize the rendering window to fill the entire rendering pane
    //CRect rectClient;
    //GetClientRect(rectClient);

    //CRect renderRect;
    //_renderWindow.GetWindowRect(renderRect);
    //ScreenToClient(renderRect);
    //_renderWindow.SetWindowPos(NULL, renderRect.left, renderRect.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

    OnAdjustLayout();
}

_Use_decl_annotations_
void RenderingPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    // Adjust layout and render
    AdjustLayout();
    Render();
}

void RenderingPane::Render()
{
    if (_graphics)
    {
        GDK::Vector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
        _graphics->Clear(clearColor);
        _graphics->ClearDepth(1.0f);
        if (_camera)
        {
            CRect clientRect;
            _renderWindow.GetClientRect(clientRect);
            _camera->SetAspect((float)clientRect.Width()/(float)clientRect.Height());

            _graphics->SetViewProjection(_camera->View(), _camera->Projection());
        }

        OnRender();
        _graphics->Present();
    }
}
