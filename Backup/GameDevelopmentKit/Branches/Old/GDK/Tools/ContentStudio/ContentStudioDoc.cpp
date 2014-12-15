#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ContentStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(ContentStudioDoc, CDocument)
END_MESSAGE_MAP()

ContentStudioDoc::ContentStudioDoc()
{
    InitializeDocument();
}

ContentStudioDoc::~ContentStudioDoc()
{
}

BOOL ContentStudioDoc::InitializeDocument()
{
    HRESULT hr = S_OK;

    _content = nullptr;
    _contentFactory = nullptr;

    // Create content factory for document
    IfFailedGotoExit(theApp.CreateContentFactory(0, _contentFactory.ReleaseAndGetAddressOf()));
    
Exit:

    if (FAILED(hr))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ContentStudioDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
        
    return InitializeDocument();
}

GDK::Content::IContent* ContentStudioDoc::GetContent()
{    
    return _content.Get(); 
}

HRESULT ContentStudioDoc::LoadArchive(_In_ CArchive& ar)
{
    DebugOut(L"Loading '%ws'...\n", ar.GetFile()->GetFileName());
    _content = nullptr;
    HRESULT hr = S_OK;

    if (_contentFactory)
    {
        std::wstring contentPath = m_strPathName;
        ComPtr<GDK::IStream> contentStream;
        HRESULT hr = S_OK;
        
        hr = CreateFileStream(ar.GetFile()->GetFilePath(), true, &contentStream);
        if (SUCCEEDED(hr))
        {
            hr = _contentFactory->CreateContent(contentStream.Get(), _content.ReleaseAndGetAddressOf());
        }
    }
    
    return hr;
}

HRESULT ContentStudioDoc::SaveArchive(_In_ CArchive& ar)
{
    DebugOut(L"Saving '%ws'...\n", ar.GetFile()->GetFileName());
    return S_OK;
}

void ContentStudioDoc::Serialize(_In_ CArchive& ar)
{
    if (ar.IsStoring())
    {
        SaveArchive(ar);
    }
    else
    {
        LoadArchive(ar);
    }
}

#ifdef _DEBUG
void ContentStudioDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void ContentStudioDoc::Dump(_In_ CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
