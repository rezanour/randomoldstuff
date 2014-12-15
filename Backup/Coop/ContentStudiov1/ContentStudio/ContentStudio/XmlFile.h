#pragma once

class XmlFile : public IUnknown
{
public:
    XmlFile()
    {
        
    }

    virtual ~XmlFile()
    {

    }

    HRESULT static OpenFile(LPCWSTR pName, IXmlReader** ppXmlReader)
    {
        HRESULT hr = S_OK;
        IStream* pFileStream = NULL;
        IXmlReader* pReader = NULL;
        IXmlReaderInput* pReaderInput = NULL;

        hr = FileStream::OpenFile(pName, &pFileStream, FALSE);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to open file");

        hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader");

        hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader");

        hr = CreateXmlReaderInputWithEncodingCodePage(pFileStream, NULL, 0 /*65001*/, FALSE, L".", &pReaderInput);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader input");

        hr = pReader->SetInput(pReaderInput);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to set reader input");

        hr = pReader->QueryInterface(IID_IXmlReader, (void**)ppXmlReader);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to QI for IXmlReader");

     Exit:

        SAFE_RELEASE(pReaderInput);
        SAFE_RELEASE(pReader);
        SAFE_RELEASE(pFileStream);

        return hr;
    }

    HRESULT static AttachFile(HANDLE hFile, IXmlReader** ppXmlReader)
    {
        HRESULT hr = S_OK;
        IStream* pFileStream = NULL;
        IXmlReader* pReader = NULL;
        IXmlReaderInput* pReaderInput = NULL;

        hr = FileStream::AttachFile(hFile, &pFileStream);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to open file");

        hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader");

        hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader");

        hr = CreateXmlReaderInputWithEncodingCodePage(pFileStream, NULL, 0 /*65001*/, FALSE, L".", &pReaderInput);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml reader input");

        hr = pReader->SetInput(pReaderInput);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to set reader input");

        hr = pReader->QueryInterface(IID_IXmlReader, (void**)ppXmlReader);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to QI for IXmlReader");

     Exit:

        SAFE_RELEASE(pReaderInput);
        SAFE_RELEASE(pReader);
        SAFE_RELEASE(pFileStream);

        return hr;
    }

    HRESULT static AttachFile(HANDLE hFile, IXmlWriter** ppXmlWriter)
    {
        HRESULT hr = S_OK;
        IStream* pFileStream = NULL;
        IXmlWriter* pWriter = NULL;

        hr = FileStream::AttachFile(hFile, &pFileStream);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to open file");

        hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**) &pWriter, NULL);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create xml writer");

        hr = pWriter->SetOutput(pFileStream);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to set writer output");

        hr = pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to set indent property on xml writer");

        hr = pWriter->QueryInterface(IID_IXmlWriter, (void**)ppXmlWriter);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to QI for IXmlWriter");

     Exit:

        SAFE_RELEASE(pWriter);
        SAFE_RELEASE(pFileStream);

        return hr;
    }

private:
    
};