#include "stdafx.h"

using namespace DirectX;

CContentStudio3DView::CContentStudio3DView() :
    m_activeCamera(0)
{

}

CContentStudio3DView::~CContentStudio3DView()
{

}

void CContentStudio3DView::OnRefresh(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;
    stde::com_ptr<IDirectXTextureResource> textureResource;
    stde::com_ptr<IGeometryResource> theGeometryResource;
    stde::com_ptr<IUnknown> theResource;
    
    ID3D11Device* device = DXUTGetD3D11Device();
    ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();

    stde::com_ptr<IContentStudioProperties> props;
    m_pDoc->GetDocumentProperties(&props);

    if (SUCCEEDED(props->GetInterface("SelectedResource", (IUnknown**)&theResource)))
    {
        // A resource was inserted..
        if (SUCCEEDED(textureResource.from<IUnknown>(theResource)))
        {
            geometryResource = nullptr;
            sprite = nullptr;
            textureResource->CreateShaderResourceView(device, &sprite);
            
            geometryResourceTexture = nullptr;
            textureResource->CreateShaderResourceView(device, &geometryResourceTexture);
        }

        if (SUCCEEDED(theGeometryResource.from<IUnknown>(theResource)))
        {
            sprite = nullptr;
            geometryResource = nullptr;
            geometryResource = DirectX::GeometricPrimitive::CreateGeometricResource(context, theGeometryResource);
        }
    }
}

void CContentStudio3DView::SetDocument(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;

    D3DXVECTOR3 vMin = D3DXVECTOR3( -1000.0f, -1000.0f, -1000.0f );
    D3DXVECTOR3 vMax = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );
    D3DXVECTOR3 vecEye( 0.0f, 0.0f, -10.0f );
    D3DXVECTOR3 vecAt( 0.0f,0.0f,0.0f );

    for (size_t i = 0; i < ARRAYSIZE(m_ViewCameras); i++)
    {
        m_ViewCameras[i].SetViewParams( &vecEye, &vecAt );
        m_ViewCameras[i].SetRotateButtons(TRUE, FALSE, FALSE);
        m_ViewCameras[i].SetScalers( 0.01f, 10 );
        m_ViewCameras[i].SetDrag( true );
        m_ViewCameras[i].SetEnableYAxisMovement( true );
        //m_ViewCameras[i].SetResetCursorAfterMove(true);
        m_ViewCameras[i].SetClipToBoundary( TRUE, &vMin, &vMax );
        m_ViewCameras[i].FrameMove( 0 );
    }
}

void CContentStudio3DView::OnUpdate(double fTime, float fElapsedTime)
{
    m_ViewCameras[m_activeCamera].FrameMove( fElapsedTime );
}

void CContentStudio3DView::OnDraw(double fTime, float fElapsedTime)
{
    D3DXMATRIX mWorld;
    D3DXMatrixIdentity(&mWorld);
    ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();
    XMMATRIX world = DXUTD3DXMatrixToXMMatrix(mWorld);
    
    //float alphaFade = (sin(fTime * 2) + 1) / 2;

    //if (alphaFade >= 1)
    //    alphaFade = 1 - FLT_EPSILON;

    ID3D11Device* device = DXUTGetD3D11Device();

    for (int i = 0; i < 1 /* 4 */; i++)
    {
        context->RSSetViewports(1, &vports[i]);

        XMMATRIX view = m_ViewCameras[i].GetViewXMMatrix();
        XMMATRIX proj = m_ViewCameras[i].GetProjXMMatrix();

        DirectX::SpriteBatch spriteBatch(context);
        CommonStates states(device);

        if (geometryResource != nullptr)
        {
            switch (i)
            {
                case 0:
                    {
                        if (geometryResourceTexture != nullptr)
                        {
                            geometryResource->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj, Colors::White, geometryResourceTexture);
                        }
                        else
                        {
                            geometryResource->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj);
                        }
                    }
                    break;
                case 1:
                    geometryResource->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj, Colors::Gray, nullptr, true);
                    break;
                case 2:
                    geometryResource->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj, Colors::White * .5);
                    break;
                case 3:
                    geometryResource->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj);
                    break;
                default:
                    break;
            }
        }

        if (majorGrid != nullptr && sprite == nullptr)
        {
            majorGrid->Draw(world * XMMatrixTranslation(0, 0, 0), view, proj, Colors::White, nullptr, false, true);
        }

        if (minorGrid != nullptr && sprite == nullptr)
        {
            minorGrid->Draw(world * XMMatrixTranslation(.5, 0, .5), view, proj, Colors::LightGray, nullptr, false, true);
        }

        // Render texture only
        if (sprite != nullptr && (i == 0))
        {
            spriteBatch.Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
            spriteBatch.Draw(sprite, XMFLOAT2(0, 0), nullptr, Colors::White, 0, XMFLOAT2(0, 0), 1.0);
            spriteBatch.End();
        }
    }
}

void CContentStudio3DView::OnLoadContent()
{
    ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();
    //majorGrid = DirectX::GeometricPrimitive::CreateGrid(context, 1, 64);
    //minorGrid = DirectX::GeometricPrimitive::CreateGrid(context, .5, 128);

    return;
}

void CContentStudio3DView::OnUnloadContent()
{
    cube = nullptr;
    sphere = nullptr;
    cylinder = nullptr;
    torus = nullptr;
    teapot = nullptr;

    sprite = nullptr;
}

void CContentStudio3DView::OnResize(UINT width, UINT height)
{
    float fAspectRatio = (FLOAT)width / (FLOAT)height;

    D3D11_VIEWPORT vp  = { 0, 0, (float)width/2, (float)height/2, 0, 1 };
    D3D11_VIEWPORT vp2 = { vp.Width, 0, (float)width/2, (float)height/2, 0, 1 };
    D3D11_VIEWPORT vp3 = { 0, vp.Height, (float)width/2, (float)height/2, 0, 1 };
    D3D11_VIEWPORT vp4 = { vp.Width, vp.Height, (float)width/2, (float)height/2, 0, 1 };

    vp.Width = width;
    vp.Height = height;

    vports[0] = vp;
    fAspectRatio = (FLOAT)vp.Width / (FLOAT)vp.Height;
    m_ViewCameras[0].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );

    vports[1] = vp2;
    fAspectRatio = (FLOAT)vp2.Width / (FLOAT)vp2.Height;
    m_ViewCameras[1].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );

    vports[2] = vp3;
    fAspectRatio = (FLOAT)vp3.Width / (FLOAT)vp3.Height;
    m_ViewCameras[2].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );

    vports[3] = vp4;
    fAspectRatio = (FLOAT)vp4.Width / (FLOAT)vp4.Height;
    m_ViewCameras[3].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );
}

void CContentStudio3DView::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                int xPos, int yPos)
{
    if (bLeftButtonDown && !m_bLastLeftButtonDownPressed)
    {
        POINT pt = {xPos, yPos};

        for (size_t i = 0; i < ARRAYSIZE(vports); i++)
        {
            RECT rc = {vports[i].TopLeftX, vports[i].TopLeftY, vports[i].Width + vports[i].TopLeftX, vports[i].Height + vports[i].TopLeftY};
            //DEBUG_PRINT("Point = (%d,%d), ViewPort = (%d,%d,%d,%d)", pt.x, pt.y, rc.left, rc.top, rc.right, rc.bottom);

            if (PtInRect(&rc, pt))
            {
                m_activeCamera = i;
                break;
            }
        }
    }

    m_bLastLeftButtonDownPressed = bLeftButtonDown;
}

void CContentStudio3DView::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown)
{

}

LRESULT CContentStudio3DView::OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    return m_ViewCameras[m_activeCamera].HandleMessages( hWnd, uMsg, wParam, lParam );
}