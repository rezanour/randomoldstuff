#pragma once

class ContentStudioDoc : public CDocument
{
protected:
    ContentStudioDoc();
    DECLARE_DYNCREATE(ContentStudioDoc)

private:
    INotifyView* _notifyView;
    std::wstring _contentFileDirectoryRoot;
    std::wstring _loadedWorldFilePath;
    bool _modified;
    bool _newDocument;

private:
    HRESULT LoadContentMappings(_In_ CArchive& ar);
    HRESULT SaveContentMappings(_In_ CArchive& ar);
    void LoadCreatableGameContent();
    BOOL InitializeDocument();
    

public:
    void FindTopLevelContent(_In_ std::wstring contentMapping, std::wstring contentMappingFullPath, _In_ std::wstring root, _In_ size_t priority);

    virtual BOOL OnNewDocument();
    virtual void Serialize(_In_ CArchive& ar);
    virtual void SetTitle(LPCTSTR lpszTitle);

    bool IsNewDocument();
    bool IsWorldModified();

    void CreateDefaultWorld(_In_ const std::shared_ptr<GDK::GraphicsDevice>& graphicsDevice);
    void OpenWorld(_In_ const std::shared_ptr<GDK::GraphicsDevice>& graphicsDevice, _In_ const std::wstring& world);
    void SetViewNotificationHandler(_In_ INotifyView* view) { _notifyView = view; }
    bool HasContentMappings() { return ContentMappings.size() > 0; }

    HRESULT SaveWorld(_In_ bool saveWithNewName);

    void SetModified(_In_ bool modified);

    void RenameWorld();

    std::wstring GameName;
    std::shared_ptr<GDK::IGameWorldEdit> GameWorld;
    std::vector<std::wstring> ContentMappings;
    std::vector<ContentPaneTreeData> ContentPaneData;

public:
    virtual ~ContentStudioDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(_In_ CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};
