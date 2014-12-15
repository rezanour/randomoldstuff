#pragma once

class ContentStudioDoc : public CDocument
{
protected:
    ContentStudioDoc();
    DECLARE_DYNCREATE(ContentStudioDoc)

private:
    INotifyView* _notifyView;

private:
    HRESULT LoadArchive(_In_ CArchive& ar);
    HRESULT SaveArchive(_In_ CArchive& ar);
    BOOL InitializeDocument();

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(_In_ CArchive& ar);
    void SetViewNotificationHandler(_In_ INotifyView* view) { _notifyView = view; }
    std::vector<std::wstring> ContentList;
    std::wstring GameName;

public:
    virtual ~ContentStudioDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(_In_ CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};
