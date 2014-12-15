#include "stdafx.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ContentStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(ContentStudioDoc, CDocument)
END_MESSAGE_MAP()

ContentStudioDoc::ContentStudioDoc() :
    _notifyView(nullptr),
    _modified(false),
    _newDocument(true)
{
    InitializeDocument();
}

ContentStudioDoc::~ContentStudioDoc()
{
}

BOOL ContentStudioDoc::InitializeDocument()
{
    GameName = L"Untitled";
    GameWorld = nullptr;

    ContentMappings.clear();
    _newDocument = true;
    _modified = false;

    return TRUE;
}

BOOL ContentStudioDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    return InitializeDocument();
}

void ContentStudioDoc::LoadCreatableGameContent()
{
#if 0 // TODO
    std::vector<std::wstring> registeredTypes = GDK::GameObject::GetRegisteredClasses();
    for (size_t i = 0; i < registeredTypes.size(); i++)
    {
        ContentPaneTreeData data;
        data.fileBasedContent = false;
        data.contentPath = registeredTypes[i];
        data.contentMapping = L"Game";
        data.expandedOnce = false;
        data.filePath = L"";
        data.isDirectory = false;
        data.rootFilePath = L"";

        // Add registered types to content pane data
        ContentPaneData.push_back(data);
    }
#endif
}

HRESULT ContentStudioDoc::LoadContentMappings(_In_ CArchive& ar)
{
    // Open .content file and mount all volumes found, mapping specified roots and logical roots
    std::string line;
    std::ifstream contentFile(ar.GetFile()->GetFilePath());
    std::wstring firstEntry;
    int32_t priority = 0;
    if (contentFile.is_open())
    {
        _contentFileDirectoryRoot = std::DirectoryRootFromPath(ar.GetFile()->GetFilePath().GetString());

        while ( contentFile.good() )
        {
            std::getline (contentFile, line);
            std::wstring parseLine = std::AnsiToWide(line);

            // Cache content mappings because we may want to provide an editing
            // experience around this later.  It also catches the case where the 
            // user wants to save their .content file to another location.
            if (parseLine.size() > 0)
            {
                ContentMappings.push_back(parseLine);
            }

            std::vector< std::basic_string<wchar_t> > x;
            size_t n = std::split(parseLine, x, L';', true);
            if (n > 0)
            {
                std::wstring root = x[0];
                std::wstring logicalRoot;

                if (root.size() > 0)
                {
                    if (n > 1)
                    {
                        logicalRoot = x[1];
                    }

                    if (PathIsRelative(root.c_str()))
                    {
                        std::wstring newRoot = _contentFileDirectoryRoot;
                        newRoot.append(L"\\");
                        newRoot.append(root);
                        root = newRoot;
                    }

                    // Mount to enable content loading
                    GDK::FileSystem::MountVolume(root, logicalRoot, priority);

                    // Enumerate top level files and folders in content location
                    FindTopLevelContent(ContentMappings[priority], root, root, priority);
                    priority++;

                    if (firstEntry.size() == 0)
                    {
                        firstEntry = root;
                    }
                }
            }
        }
        contentFile.close();
    }

    Quake2::Initialize();
    GameName = L"Content ";

    LoadCreatableGameContent();

    return S_OK;
}

_Use_decl_annotations_
void ContentStudioDoc::FindTopLevelContent(std::wstring contentMapping, std::wstring contentMappingFullPath, std::wstring root, size_t priority)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    std::wstring actualSearch = root + L"\\*.*";

    hFind = FindFirstFile(actualSearch.c_str(), &FindData);
    do
    {
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return;
        }

        std::wstring fileName (FindData.cFileName);

        if (fileName == L"." || fileName == L"..")
        {
            continue;
        }

        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            continue;
        }

        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Add directory
            ContentPaneTreeData data;
            data.priority = priority;
            data.rootFilePath = root;
            data.filePath = root + L"\\" + fileName;
            data.fileBasedContent = true;
            data.isDirectory = true;
            data.expandedOnce = false;
            data.contentMapping = contentMapping;
            data.contentPath = data.filePath.substr(contentMappingFullPath.length() + 1);
            data.contentMappingRoot = contentMappingFullPath;

            ContentPaneData.push_back(data);
        }
        else
        {
            // Add file
            ContentPaneTreeData data;
            data.priority = priority;
            data.rootFilePath = root;
            data.filePath = root + L"\\" + fileName;
            data.fileBasedContent = true;
            data.isDirectory = false;
            data.expandedOnce = false;
            data.contentMapping = contentMapping;
            data.contentPath = data.filePath.substr(contentMappingFullPath.length() + 1);
            data.contentMappingRoot = contentMappingFullPath;

            ContentPaneData.push_back(data);
        }
    } while (FindNextFile(hFind, &FindData));

    FindClose(hFind);
}

void ContentStudioDoc::SetTitle(LPCTSTR lpszTitle)
{
    if (GameWorld && GameWorld->GetName().empty())
    {
        __super::SetTitle(lpszTitle);
    }
    else
    {
        std::wstring finalTitle = GameWorld ? GameWorld->GetName().c_str() : lpszTitle;
        if (_modified)
        {
            finalTitle.append(L"*");
        }
        __super::SetTitle(finalTitle.c_str());
    }
}

bool ContentStudioDoc::IsNewDocument()
{
    return _newDocument;
}

bool ContentStudioDoc::IsWorldModified()
{
    return _modified;
}

void ContentStudioDoc::RenameWorld()
{
    NameWorldDlg nameWorld(GameWorld->GetName().c_str());
    if (nameWorld.DoModal() == IDOK)
    {
        GameWorld->SetName(nameWorld.GetName());
        SetModified(true);
    }
}

void ContentStudioDoc::CreateDefaultWorld(_In_ const std::shared_ptr<GDK::GraphicsDevice>& graphicsDevice)
{
    OpenWorld(graphicsDevice, L"");
}

void ContentStudioDoc::SetModified(const _In_ bool modified)
{
    _modified = modified;
    SetTitle(GameWorld->GetName().c_str());
}

void ContentStudioDoc::OpenWorld(_In_ const std::shared_ptr<GDK::GraphicsDevice>& graphicsDevice, _In_ const std::wstring& world)
{
    GameWorld = nullptr;

    GDK::DeviceContext deviceContext(graphicsDevice, nullptr);
    if (PathFileExists(world.c_str()))
    {
        std::ifstream file(world.c_str(), std::ios::binary);
        GameWorld = GDK::GameWorld::Create(GDK::GameWorldContent::Create(file), deviceContext);
        _loadedWorldFilePath = world;
        file.close();
        _newDocument = false;
        _modified = false;
    }
    else
    {
        GameWorld = GDK::GameWorld::Create(L"untitled", deviceContext);
        _newDocument = true;
    }

    // Set the document title
    SetTitle(GameWorld->GetName().c_str());
}

HRESULT ContentStudioDoc::SaveWorld(_In_ bool saveWithNewName)
{
    if (GameWorld)
    {
        if (_newDocument || saveWithNewName)
        {
            NameWorldDlg nameWorld;
            if (nameWorld.DoModal() == IDOK)
            {
                GameWorld->SetName(nameWorld.GetName());
            }
            else
            {
                return S_OK;
            }

            static wchar_t BASED_CODE szFilter[] = L"World Files (*.world)|";
            // Create an Open dialog; the default file name extension is ".my".
            CFileDialog fileDlg(FALSE, L"world", nameWorld.GetName().c_str(), OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilter, nullptr);
            if(fileDlg.DoModal() == IDOK )
            {
                CString pathName = fileDlg.GetPathName();
                _loadedWorldFilePath = pathName.GetBuffer();
            }
            else
            {
                // Ignore save
                return S_OK;
            }
        }

        std::ofstream file(_loadedWorldFilePath);
        GameWorld->SaveToContent()->Save(file);
        _modified = false;
        _newDocument = false;
        file.close();
        SetTitle(GameWorld->GetName().c_str());
    }
    return S_OK;
}

HRESULT ContentStudioDoc::SaveContentMappings(_In_ CArchive& ar)
{
    UNREFERENCED_PARAMETER(ar);
    for (size_t i = 0; i < ContentMappings.size(); i++)
    {
        ar.WriteString(ContentMappings[i].c_str());
        ar.WriteString(L"\r\n");
    }

    return S_OK;
}

void ContentStudioDoc::Serialize(_In_ CArchive& ar)
{
    if (ar.IsStoring())
    {
        SaveContentMappings(ar);
    }
    else
    {
        LoadContentMappings(ar);
    }
}

#ifdef _DEBUG
void ContentStudioDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void ContentStudioDoc::Dump(_In_ CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
