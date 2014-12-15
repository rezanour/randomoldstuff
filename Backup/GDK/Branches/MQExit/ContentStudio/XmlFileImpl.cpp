#include "stdafx.h"

XmlFileImpl::XmlFileImpl() :
    _refcount(1)
{

}

XmlFileImpl::~XmlFileImpl()
{

}

HRESULT STDMETHODCALLTYPE XmlFileImpl::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == __uuidof(IXmlFile))
    {
        *ppvObject = static_cast<XmlFileImpl*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE XmlFileImpl::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE XmlFileImpl::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT XmlFileImpl::Initialize(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> fileStream;
    COleVariant v;

    hr = CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&_document);
    if (SUCCEEDED(hr))
    {
        _document->put_async(VARIANT_FALSE);
        _document->put_validateOnParse(VARIANT_FALSE);
        _document->put_resolveExternals(VARIANT_FALSE);
    }

    if (SUCCEEDED(hr))
    {
        hr = CoreServices::FileStream::Create(filePath, true, &fileStream);
    }

    if (SUCCEEDED(hr))
    {
        v.vt = VT_UNKNOWN;
        hr =  fileStream->QueryInterface(IID_IUnknown, (void**)&v.punkVal);
        if (SUCCEEDED(hr))
        {
            VARIANT_BOOL isSuccessful = VARIANT_FALSE;
            hr = _document->load(v, &isSuccessful);
            if (isSuccessful != VARIANT_TRUE)
            {
                hr = E_FAIL;
            }
        }

        VariantClear(&v);
    }

    return hr;
}

HRESULT XmlFileImpl::Create(LPCWSTR filePath, IXmlFile** ppXmlFile)
{
    HRESULT hr = S_OK;

    XmlFileImpl* pXmlFile = new XmlFileImpl();
    hr = pXmlFile->Initialize(filePath);
    if (SUCCEEDED(hr))
    {
        *ppXmlFile = pXmlFile;
    }
    else
    {
        pXmlFile->Release();
    }

    return hr;
}

HRESULT XmlFileImpl::GetDocument(IXMLDOMDocument** ppDocument)
{
    if (_document == nullptr)
    {
        return E_UNEXPECTED;
    }

    return _document->QueryInterface(IID_IXMLDOMDocument, (void**)ppDocument);
}