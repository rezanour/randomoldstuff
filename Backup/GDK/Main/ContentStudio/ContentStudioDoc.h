#pragma once

class CContentStudioDoc : public CDocument, public IContentStudioDocument
{
protected:
    CContentStudioDoc();
    DECLARE_DYNCREATE(CContentStudioDoc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    void Clear();

// IContentStudioDocument
public:
    HRESULT GetDocumentProperties(IContentStudioProperties** ppProperties);
    HRESULT CreateDocument();
    void Refresh();

    HRESULT DebugCreateCubeOnPlaneProject(bool useWolfenstein3dMap);

private:
    HRESULT CreateDocumentProperties();
    HRESULT SaveDocument(LPCWSTR path, HANDLE hFile);
    HRESULT LoadDocument(LPCWSTR path);
    HRESULT WritePropertiesToConfiguration(CoreServices::ConfigurationPtr spConfiguration);
    HRESULT ReadPropertiesFromConfiguration(CoreServices::ConfigurationPtr spConfiguration);
    HRESULT PopulateDocumentObjects();
    void EnumerateEverything(CFileNode* pRootNode, IContentStudioProperties* pRoot, BOOL skipRootNode);
    
    void WriteFileProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties, IContentStudioProperties* pParentPropertied);

    void WriteGameObjectProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteModelAssetProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteMaterialAssetProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteScreenProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteTextureResourceProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteGeometryResourceProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteVisualComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteLightComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    void WriteCameraComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties);
    
    void WriteContentIndexEntry( IContentStudioProperties* pNodeProperties);

protected:
    CoreServices::ConfigurationPtr _contentIndex;
    stde::com_ptr<IContentStudioProperties> _documentProperties;
    stde::com_ptr<IContentStudioPropertiesCollection> _documentObjects;
    bool m_firstDocument;

public:
    virtual ~CContentStudioDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};
