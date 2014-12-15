#pragma once

class CFileNode
{
public:
    virtual ~CFileNode()
    {
        Destroy();
    }

    void Destroy()
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i] != nullptr)
            {
                delete children[i];
                children[i] = nullptr;
            }
        }      

        children.clear();
    }

    static void EnumerateFiles(CFileNode* pRootNode, std::wstring name, bool hidden)
    {
        HANDLE hFind;
        WIN32_FIND_DATA FindData;
        std::wstring actualSearch = name + L"\\*.*";

        hFind = FindFirstFile(actualSearch.c_str(), &FindData);
        do
        {
            if (hFind == INVALID_HANDLE_VALUE)
            {
                return;
            }

            std::wstring fileName = FindData.cFileName;

            if (fileName == L"." || fileName == L"..")
            {
                continue;
            }
            else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN && !hidden)
            {
                continue;
            }
            else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                CFileNode* pFolderNode = new CFileNode();
                pFolderNode->name = fileName;
                pFolderNode->fullPath = name + fileName;
                pFolderNode->isDirectory = true;

                pRootNode->children.push_back(pFolderNode);

                // Recurse
                EnumerateFiles(pFolderNode, /*name + L"\\" + fileName*/ pFolderNode->fullPath.c_str(), hidden);
            }
            else
            {
                CFileNode* pFileNode = new CFileNode();
                pFileNode->name = fileName;
                pFileNode->fullPath = name + L"\\" + fileName;
                pFileNode->isDirectory = false;

                pRootNode->children.push_back(pFileNode);
            }
        } while (FindNextFile(hFind, &FindData));

        FindClose(hFind);
    }

public:
    std::wstring name;
    std::wstring fullPath;
    bool isDirectory;
    std::vector<CFileNode*> children;
};