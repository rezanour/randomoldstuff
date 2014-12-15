#include "stdafx.h"

CNullContentView::CNullContentView() { }
CNullContentView::~CNullContentView() { }
void CNullContentView::OnRefresh(CContentStudioDoc* pDoc) { m_pDoc = pDoc; }
void CNullContentView::SetDocument(CContentStudioDoc* pDoc) { m_pDoc = pDoc; }
void CNullContentView::OnUpdate(double fTime, float fElapsedTime) { }
void CNullContentView::OnDraw(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext, double fTime, float fElapsedTime) { }
void CNullContentView::OnLoadContent(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext) { }
void CNullContentView::OnUnloadContent() { }
void CNullContentView::OnResize(UINT width, UINT height) { }
void CNullContentView::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                int xPos, int yPos) { }
void CNullContentView::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown) { }
LRESULT CNullContentView::OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing) { return 0; }
