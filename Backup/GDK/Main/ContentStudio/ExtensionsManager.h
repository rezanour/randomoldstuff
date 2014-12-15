#pragma once

struct UIExtensionInfo
{
    std::string name;
    stde::com_ptr<IContentStudioUIExtension> extension;
};

struct ExtensionInfo
{
    std::string name;
    std::string extensionPath;
};

struct SupportedDataExtensionInfo
{
    std::wstring extension;
    std::wstring description;
    std::string extensionPath;
};

class UIExtensionManager
{
public:
    UIExtensionManager();
    virtual ~UIExtensionManager();
    void Initialize(PCSTR filePath, unsigned int maxExtensions = 10);
    size_t GetExtensionsCount() { return _uiExtensions.size(); }
    HRESULT GetWindow(size_t index, IContentStudioWindow** ppWindow);
    HRESULT AddExtension(LPCSTR name, IContentStudioUIExtension* pExtension);
    void SetDocumentProperties(IContentStudioProperties* pProperties);
private:
    void ParseExtensions(PCSTR filePath);
    HRESULT CreateUIExtension(PCSTR path, LPCWSTR name, IContentStudioUIExtension** ppExtension);
    std::vector<UIExtensionInfo> _uiExtensions;
    std::vector<HINSTANCE> _uiExtensionDlls;
    unsigned int _maxExtensions;
};

struct DataExtensionInfo
{
    std::string name;
    stde::com_ptr<IContentStudioDataExtension> extension;
};

class DataExtensionManager
{
public:
    DataExtensionManager();
    virtual ~DataExtensionManager();
    static std::wstring GetSupportedFilesFilter();
    static HRESULT GetSupportedDataExtensionInfo(LPCSTR path, LPCWSTR name, std::vector<SupportedDataExtensionInfo>& extensionInfoCollection);
private:
    std::vector<DataExtensionInfo> _dataExtensions;
};
