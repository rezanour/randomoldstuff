#pragma once

#include "ProjectBrowerWindow.h"

class ProjectBrowserExtension : public IContentStudioUIExtension
{
public:
    ProjectBrowserExtension(ProjectBrowseType browseType);
    virtual ~ProjectBrowserExtension();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioUIExtension
    HRESULT GetWindow(_Deref_out_ IContentStudioWindow** ppWindow);
    HRESULT GetToolbar(_Deref_out_ IContentStudioToolbar** ppToolbar);
    HRESULT GetMenu(_Deref_out_ IContentStudioMenu** ppMenu);
    HRESULT SetDocumentProperties(IContentStudioProperties* pProperties);
    
private:
    ProjectBrowseType _browseType;
    stde::com_ptr<IContentStudioWindow> _window;
    long _refcount;
};

