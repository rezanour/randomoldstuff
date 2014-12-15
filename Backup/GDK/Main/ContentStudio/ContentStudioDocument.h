#pragma once

// IContentStudioDocument
class IContentStudioDocument
{
public:
    virtual HRESULT GetDocumentProperties(IContentStudioProperties** ppProperties) = 0;
};

typedef std::shared_ptr<IContentStudioDocument> IContentStudioDocumentPtr;
