#pragma once

typedef enum ProjectFileNodeType
{
    eprojectroot,
    efolder,
    efoldershortcut,
    efolderopen,
    efile,
    emodel,
    etexture,
    emesh,
    espritefont,
}ProjectFileNodeType;

class CProjectFileNode
{
public:
    CProjectFileNode() :
      nodeType(efile),
      hTreeItem(NULL),
      contentIndex(-1)
    {

    }

    virtual ~CProjectFileNode()
    {
        Destroy();
    }

    void Destroy()
    {
        for (std::size_t i = 0; i < children.size(); i++)
        {
            SAFE_DELETE(children[i]);
        }

        children.clear();
        hTreeItem = NULL;
    }
public:
    std::wstring name;
    std::vector<CProjectFileNode*> children;
    ProjectFileNodeType nodeType;
    HTREEITEM hTreeItem;      // link to the solution explorer tree control
    std::wstring backingFile; // link to the file for this item.  This can be empty if no file is backing the item
    INT contentIndex;
};

class CProjectFile
{
public:
    CProjectFile()
    {
        NewProject();
    }

    virtual ~CProjectFile()
    {
        Destroy();
    }

    void NewProject()
    {
        Destroy();

        // Add solution Root
        CProjectFileNode* pSolutionNode = new CProjectFileNode();
        pSolutionNode->name = L"Solution";
        pSolutionNode->nodeType = eprojectroot;
        nodes.children.push_back(pSolutionNode);
    }

    HRESULT SaveAs(LPCWSTR fileName, HANDLE hFile)
    {
        HRESULT hr = S_OK;

        DEBUG_PRINT("saving '%ws'...", fileName);

        IXmlWriter* pWriter = NULL;
        XmlNodeType nodeType = XmlNodeType_None;

        hr = XmlFile::AttachFile(hFile, &pWriter);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to open xml file");

        WriteElements(&nodes, pWriter);

    Exit:

        SAFE_RELEASE(pWriter);

        return hr;
    }

    void WriteElements(CProjectFileNode* pRootNode, IXmlWriter* pWriter)
    {
        HRESULT hr = S_OK;

        if (pRootNode != NULL)
        {
            if (pRootNode->name.length() > 0)
            {
                // Write start element
                hr = pWriter->WriteStartElement(NULL, NodeTypeToString(pRootNode->nodeType).c_str(), NULL);

                // Write attributes
                hr = pWriter->WriteAttributeString(NULL, L"name", NULL, pRootNode->name.c_str());
                hr = pWriter->WriteAttributeString(NULL, L"type", NULL, NodeTypeToString(pRootNode->nodeType).c_str());
                // Write source file and content index
                std::wstring sourceFile = pRootNode->backingFile + L"," + ConvertTo<std::wstring>(pRootNode->contentIndex);
                if (pRootNode->contentIndex == -1)
                {
                    // Write just the source file
                    hr = pWriter->WriteAttributeString(NULL, L"source", NULL, pRootNode->backingFile.c_str());
                }
                else
                {
                    // Write source file and content index
                    hr = pWriter->WriteAttributeString(NULL, L"source", NULL, sourceFile.c_str());
                }
            }
            // Node is a folder and has children, iterate them and recurse
            if (pRootNode->children.size() > 0)
            {
                // Iterate over childern
                std::vector<CProjectFileNode*>::iterator fileNodeIterator;
                for(fileNodeIterator = pRootNode->children.begin();  fileNodeIterator != pRootNode->children.end(); fileNodeIterator++)
                {
                    // Recurse for each child found
                    WriteElements(*fileNodeIterator, pWriter);
                }
            }

            if (pRootNode->name.length() > 0)
            {
                // Write end element
                hr = pWriter->WriteEndElement();
            }
        }
    }

    void EnumerateElements(CProjectFileNode* pRootNode, IXmlReader* pReader)
    {
        HRESULT hr = S_OK;
        XmlNodeType nodeType = XmlNodeType_None;
        LPCWSTR theValue = NULL;
        LPCWSTR theAttribute = NULL;
        UINT cchtheValue = 0;
        UINT cchtheAttribute = 0;
        UINT numAttributes = 0;

        while (S_OK == (hr = pReader->Read(&nodeType)))
        {
            CProjectFileNode* newNode = NULL;

            if (nodeType == XmlNodeType_Element)
            {
                BOOL hasChildren = FALSE;
                newNode = new CProjectFileNode();
                newNode->nodeType = efile;

                // Read name
                hr = pReader->GetLocalName(&theValue, &cchtheValue); 
                if (SUCCEEDED(hr))
                {
                    hasChildren = !pReader->IsEmptyElement();
                }

                // Read attributes
                hr = pReader->GetAttributeCount(&numAttributes);
                if (SUCCEEDED(hr))
                {
                    if (numAttributes > 0)
                    {
                        hr = pReader->MoveToFirstAttribute();
                        for (UINT i = 0; i < numAttributes; i++)
                        {
                            hr = pReader->GetLocalName(&theValue, &cchtheValue); 
                            hr = pReader->GetValue(&theAttribute, &cchtheAttribute);
                            if (SUCCEEDED(hr))
                            {
                                std::wstring ntype;
                                ntype = theValue;

                                if (ntype == L"name")
                                {
                                    newNode->name = theAttribute;
                                }

                                if (ntype == L"type")
                                {
                                    newNode->nodeType = NodeTypeFromString(std::wstring(theAttribute));
                                }

                                if (ntype == L"source")
                                {
                                    std::wstring source = theAttribute;
                                    size_t pos = source.find_last_of(L",");
                                    // If a content id is present, parse it out
                                    if (pos != std::wstring::npos)
                                    {
                                        newNode->backingFile = source.substr(0, pos);
                                        std::wstring id = source.substr(pos + 1, std::wstring::npos);
                                        newNode->contentIndex = ConvertTo<int>(id);
                                    }
                                    else
                                    {
                                        newNode->backingFile = theAttribute;
                                    }
                                }
                            }

                            hr = pReader->MoveToNextAttribute();
                        }
                    }
                }

                // Add node to children list
                pRootNode->children.push_back(newNode);

                if (hasChildren)
                {
                    EnumerateElements(newNode, pReader);
                }

                newNode = NULL;
            }

            if (nodeType == XmlNodeType_EndElement)
            {
                break;
            }
        }
    }

    HRESULT Load(LPCWSTR fileName, HANDLE hFile)
    {
        Destroy();

        HRESULT hr = S_OK;

        DEBUG_PRINT("loading '%ws'...", fileName);

        IXmlReader* pReader = NULL;
        XmlNodeType nodeType = XmlNodeType_None;

        hr = XmlFile::AttachFile(hFile, &pReader);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to open xml file");

        EnumerateElements(&nodes, pReader);

    Exit:

        SAFE_RELEASE(pReader);

        return hr;
    }

    void Destroy()
    {
        nodes.Destroy();
    }

private:
    ProjectFileNodeType NodeTypeFromString(std::wstring& typeStr)
    {
        ProjectFileNodeType nodeType = efile;

        if (typeStr == L"projectroot")
        {
            nodeType = eprojectroot;
        }
        else if (typeStr == L"folder")
        {
            nodeType = efolder;
        }
        else if (typeStr == L"foldershortcut")
        {
            nodeType = efoldershortcut;
        }
        else if (typeStr == L"folderopen")
        {
            nodeType = efolderopen;
        }
        else if (typeStr == L"file")
        {
            nodeType = efile;
        }
        else if (typeStr == L"model")
        {
            nodeType = emodel;
        }
        else if (typeStr == L"mesh")
        {
            nodeType = emesh;
        }
        else if (typeStr == L"texture")
        {
            nodeType = etexture;
        }
        else if (typeStr == L"spritefont")
        {
            nodeType = espritefont;
        }

        return nodeType;
    }

    std::wstring NodeTypeToString(ProjectFileNodeType& nodeType)
    {
        std::wstring typeStr = L"unknown";

        if (nodeType == eprojectroot)
        {
            typeStr = L"projectroot";
        }
        else if (nodeType == efolder)
        {
            typeStr = L"folder";
        }
        else if (nodeType == efoldershortcut)
        {
            typeStr = L"foldershortcut";
        }
        else if (nodeType == efolderopen)
        {
            typeStr = L"folderopen";
        }
        else if (nodeType == efile)
        {
            typeStr = L"file";
        }
        else if (nodeType == emodel)
        {
            typeStr = L"model";
        }
        else if (nodeType == emesh)
        {
            typeStr = L"mesh";
        }
        else if (nodeType == etexture)
        {
            typeStr = L"texture";
        }
        else if (nodeType == espritefont)
        {
            typeStr = L"spritefont";
        }

        return typeStr;
    }

public:
    CProjectFileNode nodes;
};