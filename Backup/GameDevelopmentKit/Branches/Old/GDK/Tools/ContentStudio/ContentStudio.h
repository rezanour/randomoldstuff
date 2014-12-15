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
    std::vector<ComponentDescription> _components;
    std::vector<size_t> _zeroBasedGraphicsComponents;
    std::vector<size_t> _zeroBasedGamesComponents;
    std::vector<size_t> _zeroBasedContentComponents;

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual BOOL OnIdle(LONG lCount);

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

    HRESULT CreateGraphicsDevice(_In_ size_t index, _In_ HWND hWnd, _COM_Outptr_ IGraphicsDevice** graphicsDevice);
    HRESULT CreateGameFactory(_In_ size_t index, IGameFactory** gameFactory);
    HRESULT CreateContentFactory(_In_ size_t index, _COM_Outptr_ GDK::Content::IContentFactory** contentFactory);

    void ChangeRenderingMode(_In_ RenderingMode mode) { _renderingMode = mode; }
    RenderingMode GetCurrentRenderingMode() { return _renderingMode; }
    std::vector<ComponentDescription>* GetComponentDescriptions() {return &_components;}

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern ContentStudioApp theApp;
