#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

class CContentStudioApp : public CWinAppEx
{
public:
    CContentStudioApp();

public:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle(LONG lCount);
    virtual int ExitInstance();

    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;
    ULONG_PTR m_gdiplusToken;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CContentStudioApp theApp;
