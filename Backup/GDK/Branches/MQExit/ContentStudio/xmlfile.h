#pragma once

[uuid("396352a7-a5e9-4c25-b3aa-35b9031174b4")]
struct IXmlFile : public IUnknown
{
    virtual HRESULT GetDocument(IXMLDOMDocument** ppDocument) = 0;
};

class XmlFileImpl : public IXmlFile
{
public:
    XmlFileImpl();
    virtual ~XmlFileImpl();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT Initialize(LPCWSTR filePath);

    // IXmlFile
    HRESULT GetDocument(IXMLDOMDocument** ppDocument);

public:
    static HRESULT Create(LPCWSTR filePath, IXmlFile** ppXmlFile);

private:
    long _refcount;
    stde::com_ptr<IXMLDOMDocument> _document;
};