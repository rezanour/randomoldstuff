#pragma once

enum ContentStudioContentType
{
    unknownContent,
    modelContent,
    textureContent,
    spritefontContent,
};

class CContentStudioDoc : public CDocument
{
protected:
    CContentStudioDoc();
    DECLARE_DYNCREATE(CContentStudioDoc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    void Destroy();

public:
    virtual ~CContentStudioDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

public:
    void AddContent(LPCWSTR szFilePath, IContentContainerPtr pContainer);
    void UpdateCurrentContent(LPCWSTR newContentFile, INT newContentIndex);

public:
    ContentStudioContentType m_currentlySelectedContentType;
    bool m_bContentUpdated;
    bool m_bEnableWireFrameNoCull;
    bool m_bEnableWireFrame;
    CProjectFile m_ProjectFile;
    std::vector<IModelContentPtr> m_modelContentCollection;
    IModelContentPtr m_currentModelContent;
    ITextureContentPtr m_currentTextureContent;
    ISpriteFontContentPtr m_currentSpriteFontContent;
    INT m_currentContentIndex;
    std::wstring m_currentModelContentFile;
    std::wstring m_lastModelContentFile;

    std::wstring m_currentTextureContentFile;
    std::wstring m_lastTextureContentFile;

    std::wstring m_currentSpriteFontContentFile;
    std::wstring m_lastSpriteFontContentFile;

    std::wstring GetContentFilesPath();
    std::wstring GetContentRootPath();

    void AddContent(LPCWSTR szFilePath, ISpriteFontContentPtr pContent);

private:
    void AddContent(LPCWSTR szFilePath, IModelContentPtr pContent);
    void AddContent(LPCWSTR szFilePath, ITextureContentPtr pContent);

protected:
    DECLARE_MESSAGE_MAP()
};
