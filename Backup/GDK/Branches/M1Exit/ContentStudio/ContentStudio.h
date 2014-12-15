#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

enum RenderingMode
{
    Render2D,
    Render3D
};

class CContentStudioApp : public CWinAppEx
{
public:
    CContentStudioApp();

public:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle(LONG lCount);
    virtual int ExitInstance();

    void ChangeRenderingMode(RenderingMode mode) { m_renderingMode = mode; }
    RenderingMode GetCurrentRenderingMode() { return m_renderingMode; }

    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;
    ULONG_PTR m_gdiplusToken;
    RenderingMode m_renderingMode;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CContentStudioApp theApp;

void SetCMFCPropertyGridCtrlWidth(CMFCPropertyGridCtrl& ctrl, int width);
