#include "stdafx.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ContentStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(ContentStudioDoc, CDocument)
END_MESSAGE_MAP()

ContentStudioDoc::ContentStudioDoc()
{
    InitializeDocument();
}

ContentStudioDoc::~ContentStudioDoc()
{
}

BOOL ContentStudioDoc::InitializeDocument()
{
    ContentList.clear();
    GameName = L"Untitled";

    return TRUE;
}

BOOL ContentStudioDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
        
    return InitializeDocument();
}

HRESULT ContentStudioDoc::LoadArchive(_In_ CArchive& ar)
{
    // TODO: Update to call into game to get a proper list of content names
    GameName = L"Quake 2";

    ContentList.clear();
    ContentList.push_back(L"enemies\\soldier.object\\primary.texture");
    ContentList.push_back(L"enemies\\soldier.object\\mesh.geometry");

    // Open .content file and mount all volumes found, mapping specified roots and logical roots
    std::string line;
    std::ifstream contentFile(ar.GetFile()->GetFilePath());
    if (contentFile.is_open())
    {
        std::wstring contentFileDirectoryRoot = std::DirectoryRootFromPath(ar.GetFile()->GetFilePath().GetString());

        while ( contentFile.good() )
        {
            std::getline (contentFile, line);
            std::wstring parseLine = std::AnsiToWide(line);

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
                        std::wstring newRoot = contentFileDirectoryRoot;
                        newRoot.append(L"\\");
                        newRoot.append(root);
                        root = newRoot;
                    }

                    GDK::Content::MountContentVolume(root, logicalRoot, 1);
                }
            }
        }
        contentFile.close();
    }

    return S_OK;
}

HRESULT ContentStudioDoc::SaveArchive(_In_ CArchive& ar)
{
    UNREFERENCED_PARAMETER(ar);
    return S_OK;
}

void ContentStudioDoc::Serialize(_In_ CArchive& ar)
{
    if (ar.IsStoring())
    {
        SaveArchive(ar);
    }
    else
    {
        LoadArchive(ar);
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
