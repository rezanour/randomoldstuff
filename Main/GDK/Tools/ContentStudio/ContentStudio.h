#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

enum RenderingMode
{
    RenderSolid,
    RenderWireframe
};

typedef void (*PRENDERCALLBACK)(void*);

class ContentStudioApp : public CWinAppEx
{
public:
    ContentStudioApp();

private:
    RenderingMode _renderingMode;
    PRENDERCALLBACK _renderCallback;
    void* _renderCallbackContext;
    ULONG_PTR _gdiplusToken;

    // Game time values
    float _totalElapsedTime;
    float _elapsedTime;
    bool _firstUpdate;
    LARGE_INTEGER _performanceFrequency;
    LARGE_INTEGER _startCounter;
    LARGE_INTEGER _lastCounter;
    LARGE_INTEGER _currentCounter;

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual BOOL OnIdle(_In_ LONG lCount);

    void SetRenderCallback(_In_ PRENDERCALLBACK renderCallback, _In_ void* renderCallbackContext) { _renderCallback = renderCallback; _renderCallbackContext = renderCallbackContext; }
    void InvokeRenderCallback();

    void UpdateGameTime();
    float GetTotalElapsedGameTime() {return _totalElapsedTime;}
    float GetElapsedGameTime() {return _elapsedTime;}

    UINT  _applicationStyleSetting;
    BOOL  _hicolorIcons;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    void ChangeRenderingMode(_In_ RenderingMode mode) { _renderingMode = mode; }
    RenderingMode GetCurrentRenderingMode() { return _renderingMode; }

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern ContentStudioApp theApp;
