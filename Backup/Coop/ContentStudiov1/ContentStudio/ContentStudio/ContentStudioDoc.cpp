#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CContentStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CContentStudioDoc, CDocument)
END_MESSAGE_MAP()

CContentStudioDoc::CContentStudioDoc()
    : m_bEnableWireFrame(false), 
    m_bEnableWireFrameNoCull(false),
    m_currentContentIndex(-1),
    m_bContentUpdated(false),
    m_currentlySelectedContentType(unknownContent)
{
}

CContentStudioDoc::~CContentStudioDoc()
{
    Destroy();
}

BOOL CContentStudioDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    Destroy();

    m_ProjectFile.NewProject();

    return TRUE;
}

void CContentStudioDoc::UpdateCurrentContent(LPCWSTR newContentFile, INT newContentIndex)
{
    std::wstring newFile = newContentFile;
    m_bContentUpdated = false;

    if (newFile.length() > 0)
    {
        if (newFile.rfind(L".mc") != std::wstring::npos)
        {
            m_currentlySelectedContentType = modelContent;
            m_currentModelContentFile = newContentFile;
            m_currentModelContent.reset();
            m_currentModelContent = CModelContent::FromFile((GetContentRootPath() + m_currentModelContentFile).c_str());
            m_bContentUpdated = true;
        }
        else if (newFile.rfind(L".tc") != std::wstring::npos)
        {
            m_currentlySelectedContentType = textureContent;
            m_currentTextureContentFile = newContentFile;
            m_currentTextureContent = CTextureContent::FromFile((GetContentRootPath() + m_currentTextureContentFile).c_str());
            m_currentTextureContent->SetTextureName(FileFromFilePath(newContentFile, FALSE));
            m_bContentUpdated = true;
        }
        else if (newFile.rfind(L".fc") != std::wstring::npos)
        {
            m_currentlySelectedContentType = spritefontContent;
            m_currentSpriteFontContentFile = newContentFile;
            m_currentSpriteFontContent = CSpriteFontContent::FromFile((GetContentRootPath() + m_currentSpriteFontContentFile).c_str());
            m_currentSpriteFontContent->SetSpriteFontName(FileFromFilePath(newContentFile, FALSE));
            m_bContentUpdated = true;
        }
        else
        {
            m_currentlySelectedContentType = unknownContent;
            m_bContentUpdated = true;
        }
    }

    if (newContentIndex == -1)
    {
        //m_bContentUpdated = false;
    }

    m_currentContentIndex = newContentIndex;
}

std::wstring CContentStudioDoc::GetContentFilesPath()
{
    std::wstring contentFilesPath = GetContentRootPath() + L"ContentStudioFiles\\";
    return contentFilesPath;
}

std::wstring CContentStudioDoc::GetContentRootPath()
{
    std::wstring contentRootPath = FilePathOnly(GetPathName());
    return contentRootPath;
}

void CContentStudioDoc::AddContent(LPCWSTR szFilePath, IContentContainerPtr pContainer)
{
    if (pContainer == nullptr)
        return;

    // check to see if the content container has both model and textures
    if (pContainer->GetModelContent(0) && pContainer->GetTextureContent(0))
    {
        // TODO: Add textures as children of model content
    }

    // Add all model content
    IModelContentPtr pModelContent;
    int m = 0;
    do
    {
        pModelContent = pContainer->GetModelContent(m++);
        AddContent(szFilePath, pModelContent);
    } while (pModelContent != nullptr);

    // Add all texture content
    ITextureContentPtr pTextureContent;
    int t = 0;
    do
    {
        pTextureContent = pContainer->GetTextureContent(t++);
        AddContent(szFilePath, pTextureContent);
    } while (pTextureContent != nullptr);

    UpdateAllViews(NULL, ID_REFRESH_SOLUTION_EXPLORER);
}

void CContentStudioDoc::AddContent(LPCWSTR szFilePath, ISpriteFontContentPtr pContent)
{
    if (pContent == nullptr)
        return;

    // If the content project has not yet been saved, prompt the user to specify a save location
    if (GetPathName().IsEmpty())
    {
        AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVE_AS);
    }

    // Always set the modified bit to ensure the application doesn't get closed without
    // saving changes
    SetModifiedFlag();

    // Create the ContentStudioFiles directory
    CreateDirectory(GetContentFilesPath().c_str(), NULL);

    // Add content file name as a folder root object
    
    CProjectFileNode* pRootNode = new CProjectFileNode();
    std::wstring name;

    name = FileFromFilePath(szFilePath, FALSE);

    // Set the backing data file for the saved .fc content file
    pRootNode->backingFile = L"ContentStudioFiles\\" + name + L".fc";
    pRootNode->name = name;
    pRootNode->nodeType = espritefont;

    // save content backing file
    pContent->SaveToFile((GetContentRootPath() + pRootNode->backingFile).c_str());

    m_ProjectFile.nodes.children[0]->children.push_back(pRootNode);

    UpdateAllViews(NULL, ID_REFRESH_SOLUTION_EXPLORER);
}

void CContentStudioDoc::AddContent(LPCWSTR szFilePath, ITextureContentPtr pContent)
{
    if (pContent == nullptr)
        return;

    // If the content project has not yet been saved, prompt the user to specify a save location
    if (GetPathName().IsEmpty())
    {
        AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVE_AS);
    }

    // Always set the modified bit to ensure the application doesn't get closed without
    // saving changes
    SetModifiedFlag();

    // Create the ContentStudioFiles directory
    CreateDirectory(GetContentFilesPath().c_str(), NULL);

    // Add content file name as a folder root object
    
    CProjectFileNode* pRootNode = new CProjectFileNode();
    std::wstring name;

    // Get the texture name, if no texture name was set, use the file name as the
    // texture name
    pContent->GetTextureName(name);
    if (name.length() == 0)
    {
        name = FileFromFilePath(szFilePath, FALSE);
    }

    // Set the backing data file for the saved .tc content file
    pRootNode->backingFile = L"ContentStudioFiles\\" + name + L".tc";
    pRootNode->name = name;
    pRootNode->nodeType = etexture;

    // save content backing file
    pContent->SaveToFile((GetContentRootPath() + pRootNode->backingFile).c_str());

    m_ProjectFile.nodes.children[0]->children.push_back(pRootNode);
}

void CContentStudioDoc::AddContent(LPCWSTR szFilePath, IModelContentPtr pContent)
{
    if (pContent == nullptr)
        return;

    // If the content project has not yet been saved, prompt the user to specify a save location
    if (GetPathName().IsEmpty())
    {
        AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVE_AS);
    }

    // Always set the modified bit to ensure the application doesn't get closed without
    // saving changes
    SetModifiedFlag();

    // Create the ContentStudioFiles directory
    CreateDirectory(GetContentFilesPath().c_str(), NULL);

    // Add content file name as a folder root object
    std::wstring name = FileFromFilePath(szFilePath, FALSE);
    CProjectFileNode* pRootNode = new CProjectFileNode();
    pRootNode->name = name;
    pRootNode->nodeType = emodel;

    // Set the backing data file for the saved .mc content file
    pRootNode->backingFile = L"ContentStudioFiles\\" + name + L".mc";

    // save content backing file
    pContent->SaveToFile((GetContentRootPath() + pRootNode->backingFile).c_str());

    std::vector<std::string> meshNames;
    pContent->GetMeshNames(meshNames);

    for (UINT i = 0; i < meshNames.size(); i++)
    {
        CProjectFileNode* pNode = new CProjectFileNode();
        pNode->name = ConvertTo<std::wstring>(meshNames[i].c_str());
        pNode->nodeType = emesh;
        pNode->contentIndex = i;
        pNode->backingFile = pRootNode->backingFile;

        pRootNode->children.push_back(pNode);
    }

    m_ProjectFile.nodes.children[0]->children.push_back(pRootNode);
}

void CContentStudioDoc::Destroy()
{
    m_ProjectFile.Destroy();
    m_modelContentCollection.clear();
    m_currentModelContent.reset();
    m_currentTextureContent.reset();
    m_currentSpriteFontContent.reset();
}

void CContentStudioDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        m_ProjectFile.SaveAs(ar.m_strFileName, ar.GetFile()->m_hFile);
    }
    else
    {
        m_ProjectFile.Destroy();
        m_ProjectFile.Load(ar.m_strFileName, ar.GetFile()->m_hFile);
    }
}

#ifdef _DEBUG
void CContentStudioDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CContentStudioDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
