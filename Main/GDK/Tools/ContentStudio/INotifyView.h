#pragma once

struct ContentPaneTreeData
{
    size_t priority;
    std::wstring rootFilePath;
    std::wstring filePath;
    std::wstring contentPath;
    std::wstring contentMapping;
    std::wstring contentMappingRoot;
    bool fileBasedContent;
    bool isDirectory;
    bool expandedOnce;
};

class INotifyView
{
public:
    virtual void OnDocumentChanged() = 0;
    virtual void OnContentSelectionChanged(_In_ const ContentPaneTreeData& selectedContent) = 0;
    virtual void OnContentSelectionOpened() = 0;
    virtual void OnContentExpanded(_In_ const ContentPaneTreeData& selectedContent) = 0;
    virtual void OnCreateObject(_In_ const std::wstring& name) = 0;
};
