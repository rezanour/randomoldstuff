#include "stdafx.h"

std::istream& operator>> (std::istream& stream, ExtensionInfo& foo)
{
    // grab the stream as a single string using getline
    std::string s;
    std::getline( stream, s );

    // create a string stream and use getline to extract comma delimited fields
    std::istringstream ss( s );
    std::getline( ss, foo.name, ',' );
    std::getline( ss, foo.extensionPath, ',' );

    return stream;
}

UIExtensionManager::UIExtensionManager()
{

}

UIExtensionManager::~UIExtensionManager()
{
    /*
    for(size_t i = 0; i < _uiExtensionDlls.size(); i++)
    {
        ::FreeLibrary(_uiExtensionDlls[i]);
    }
    */
}

void UIExtensionManager::Initialize(PCSTR filePath, unsigned int maxExtensions)
{
    _maxExtensions = maxExtensions;
    ParseExtensions(filePath);
}

HRESULT UIExtensionManager::GetWindow(size_t index, IContentStudioWindow** ppWindow) 
{
    HRESULT hr = S_OK;

    // TODO: validate index against extensions collection and non-nullptr ppWindow

    _uiExtensions[index].extension->GetWindow(ppWindow); 
    return hr;
}

HRESULT UIExtensionManager::AddExtension(LPCSTR name, IContentStudioUIExtension* pExtension)
{
    if (!pExtension)
    {
        return E_INVALIDARG;
    }
    
    UIExtensionInfo info;
    info.name = name;
    info.extension = pExtension;
    _uiExtensions.push_back(info);
    
    return S_OK;
}

void UIExtensionManager::SetDocumentProperties(IContentStudioProperties* pProperties)
{
    for (size_t i = 0; i < _uiExtensions.size(); i++)
    {
        _uiExtensions[i].extension->SetDocumentProperties(pProperties);
    }
}

void UIExtensionManager::ParseExtensions(PCSTR filePath)
{
    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr spConfiguration;
    hr = CoreServices::Configuration::Load(stde::to_wstring(std::string(filePath)), &spConfiguration);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < _maxExtensions; i++)
        {
            std::string extensionName = "UIExtension" + std::to_string((_Longlong)i);
            ExtensionInfo extensionInfo;
            extensionInfo = spConfiguration->GetValue<ExtensionInfo>(extensionName, ExtensionInfo());
            
            if (!extensionInfo.name.empty() && !extensionInfo.extensionPath.empty())
            {
                // attempt to load the extension dll and obtain the extension interface
                UIExtensionInfo eInfo;
                eInfo.name = extensionInfo.name;

                if (SUCCEEDED(CreateUIExtension(extensionInfo.extensionPath.c_str(), stde::to_wstring(eInfo.name).c_str(), &eInfo.extension)))
                {
                    hr = AddExtension(extensionInfo.name.c_str(), eInfo.extension);
                }
            }
        }
    }
}

HRESULT UIExtensionManager::CreateUIExtension(LPCSTR path, LPCWSTR name, IContentStudioUIExtension** ppExtension)
{
    HRESULT hr = S_OK;
    HINSTANCE hExtensionDLL = ::LoadLibraryA(path);
    if (hExtensionDLL != NULL)
    {
        PCREATEUIEXTENSION CreateUIExtension = (PCREATEUIEXTENSION)GetProcAddress(hExtensionDLL, CreateUIExtensionExport);
        if (CreateUIExtension == NULL)
        {
            DEBUG_PRINT("CreateUIExtension is not supported by extension (%s)", path);
            hr = E_NOTIMPL;
        }
        else
        {
            hr = CreateUIExtension(name, ppExtension);
            if (FAILED(hr))
            {
                DEBUG_PRINT("CreateUIExtension from extension (%s), failed to return an extension for name (%ws), hr = 0x%lx", path,name, hr);
            }
        }

        if (SUCCEEDED(hr))
        {
            _uiExtensionDlls.push_back(hExtensionDLL);
        }
        else
        {
            ::FreeLibrary(hExtensionDLL);
        }
    }
    else
    {
        hr = E_NOTIMPL;
    }

    return hr;
}

DataExtensionManager::DataExtensionManager()
{

}

DataExtensionManager::~DataExtensionManager()
{

}

std::wstring DataExtensionManager::GetSupportedFilesFilter()
{
    std::wstring supportedFilesFilter;
    std::vector<SupportedDataExtensionInfo> supportedFileExtensions;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr spConfiguration;
    hr = CoreServices::Configuration::Load(L"ContentStudio.ini", &spConfiguration);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < 50; i++)
        {
            std::string extensionName = "DataExtension" + std::to_string((_Longlong)i);

            ExtensionInfo extensionInfo;
            extensionInfo = spConfiguration->GetValue<ExtensionInfo>(extensionName, ExtensionInfo());
            
            if (!extensionInfo.name.empty() && !extensionInfo.extensionPath.empty())
            {
                GetSupportedDataExtensionInfo(extensionInfo.extensionPath.c_str(), stde::to_wstring(extensionInfo.name).c_str(), supportedFileExtensions);
            }
        }
    }

    for (size_t i = 0; i < supportedFileExtensions.size(); i++)
    {
        supportedFilesFilter += supportedFileExtensions[i].description + L" Files " + L"(*." + supportedFileExtensions[i].extension + L")|*." + supportedFileExtensions[i].extension + L"|";
    }

    supportedFilesFilter += L"All Files (*.*)|*.*||";

    return supportedFilesFilter;
}

HRESULT DataExtensionManager::GetSupportedDataExtensionInfo(LPCSTR path, LPCWSTR name, std::vector<SupportedDataExtensionInfo>& extensionInfoCollection)
{
    HRESULT hr = S_OK;
    HINSTANCE hExtensionDLL = ::LoadLibraryA(path);
    if (hExtensionDLL != NULL)
    {
        PCREATEDATAEXTENSION CreateDataExtension = (PCREATEDATAEXTENSION)GetProcAddress(hExtensionDLL, CreateDataExtensionExport);
        if (CreateDataExtension == NULL)
        {
            DEBUG_PRINT("CreateDataExtension is not supported by extension (%s)", path);
            hr = E_NOTIMPL;
        }
        else
        {
            stde::com_ptr<IContentStudioDataExtension> spExtension;
            hr = CreateDataExtension(name, &spExtension);
            if SUCCEEDED(hr)
            {
                FileExtensionDescription* pDescriptions = nullptr;
                size_t numDescriptions = 0;
                hr = spExtension->GetSupportedFileDescriptions(&pDescriptions, numDescriptions);
                if (SUCCEEDED(hr))
                {
                    for (size_t i = 0; i < numDescriptions; i++)
                    {
                        SupportedDataExtensionInfo info;
                        info.extension = pDescriptions[i].extension;
                        info.description = pDescriptions[i].description;
                        extensionInfoCollection.push_back(info);
                    }
                }
            }
            else
            {
                DEBUG_PRINT("CreateDataExtension from extension (%s), failed to return an extension for name (%ws), hr = 0x%lx", path,name, hr);
            }
        }
        ::FreeLibrary(hExtensionDLL);
    }
    else
    {
        hr = E_NOTIMPL;
    }

    return hr;
}

