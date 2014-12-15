#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CContentStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CContentStudioDoc, CDocument)
END_MESSAGE_MAP()

CContentStudioDoc::CContentStudioDoc() : 
    m_firstDocument(true)
{
    CreateDocumentProperties();
}

CContentStudioDoc::~CContentStudioDoc()
{

}

BOOL CContentStudioDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    Clear();
    // Ensure that the document objects property exists in the collection
    _documentProperties->SetInterface(DocumentObjectsProperty, _documentObjects);
    Refresh();

    return TRUE;
}

void CContentStudioDoc::Refresh()
{
    PopulateDocumentObjects();
}

HRESULT CContentStudioDoc::CreateDocumentProperties()
{
    HRESULT hr = S_OK;

    // Create document properties
    hr = ContentStudioProperties::Create(&_documentProperties);

    // Create document objects interface and set DocumentObjectsProperty to
    // this value.
    if (SUCCEEDED(hr))
    {
        ContentStudioPropertiesCollection* pPropsCollection = new ContentStudioPropertiesCollection();
        if (pPropsCollection != nullptr)
        {
            hr = pPropsCollection->QueryInterface(IID_IUnknown, (void**)&_documentObjects);
            pPropsCollection->Release();
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            hr = _documentProperties->SetInterface(DocumentObjectsProperty, _documentObjects);
        }
    }

    return hr;
}

void CContentStudioDoc::WriteFileProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties, IContentStudioProperties* pParentProperties)
{
    HRESULT hr = S_OK;

    pNodeProperties->SetStringW(FullPathProperty, (LPWSTR)pNode->fullPath.c_str());
    pNodeProperties->SetStringW(NameProperty, (LPWSTR)pNode->name.c_str());
    pNodeProperties->SetInt32(IsFolderProperty, pNode->isDirectory ? 1 : 0);
}

void CContentStudioDoc::WriteGameObjectProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .gameobject extensions
    if (pNode->fullPath.rfind(L".gameobject") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ObjectContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ObjectType::GameObjectInstanceObject);
}

void CContentStudioDoc::WriteVisualComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .visualcomponent extensions
    if (pNode->fullPath.rfind(L".visualcomponent") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ComponentContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ComponentType::VisualComponent);
}

void CContentStudioDoc::WriteLightComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .lightcomponent extensions
    if (pNode->fullPath.rfind(L".lightcomponent") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ComponentContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ComponentType::LightComponent);
}

void CContentStudioDoc::WriteCameraComponentProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .cameracomponent extensions
    if (pNode->fullPath.rfind(L".cameracomponent") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ComponentContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ComponentType::CameraComponent);
}

void CContentStudioDoc::WriteModelAssetProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .modelasset extensions
    if (pNode->fullPath.rfind(L".modelasset") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::AssetContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::AssetType::StaticModelAsset);
}

void CContentStudioDoc::WriteMaterialAssetProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .materialasset extensions
    if (pNode->fullPath.rfind(L".materialasset") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::AssetContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::AssetType::MaterialAsset);
}

void CContentStudioDoc::WriteScreenProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .screen extensions
    if (pNode->fullPath.rfind(L".screen") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = CoreServices::Configuration::Load(fileStream, &file);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = file->GetStringValue(NameProperty, "");
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }
    }

    pNodeProperties->SetString(IdProperty, (LPSTR)file->GetStringValue(IdProperty).c_str());
    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ObjectContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ObjectType::ScreenObject);
}

void CContentStudioDoc::WriteTextureResourceProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .textureresource extensions
    if (pNode->fullPath.rfind(L".textureresource") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr file;
    stde::com_ptr<IStream> fileStream;

    ULONG cbRead = 0;
    TEXTURE_RESOURCE_FILEHEADER trf = {0};

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = fileStream->Read(&trf, sizeof(trf), &cbRead);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = trf.header.Name;
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }

        pNodeProperties->SetUInt64(IdProperty, trf.header.Id);
        pNodeProperties->SetInt32(WidthProperty, (int)trf.metadata.width);
        pNodeProperties->SetInt32(HeightProperty, (int)trf.metadata.height);
        pNodeProperties->SetInt32(FormatProperty, (int)trf.metadata.format);
        pNodeProperties->SetInt32(ArraySizeProperty, (int)trf.metadata.arraySize);
        pNodeProperties->SetInt32(DepthProperty, (int)trf.metadata.depth);
        pNodeProperties->SetInt32(MipLevelsProperty, (int)trf.metadata.mipLevels);
        pNodeProperties->SetInt32(MiscProperty, (int)trf.metadata.miscFlags);
        pNodeProperties->SetInt32(DimensionProperty, (int)trf.metadata.dimension);
    }

    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ResourceContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ResourceType::Texture2DResource);
}

void CContentStudioDoc::WriteGeometryResourceProperties(CFileNode* pNode, IContentStudioProperties* pNodeProperties)
{
    // only process files with .geometryresource extensions
    if (pNode->fullPath.rfind(L".geometryresource") == std::wstring::npos)
        return;

    HRESULT hr = S_OK;
    stde::com_ptr<IStream> fileStream;
    ULONG cbRead = 0;
    GEOMETRY_RESOURCE_FILEHEADER grf = {0};

    hr = CoreServices::FileStream::Create(pNode->fullPath, true, &fileStream);
    if (SUCCEEDED(hr))
    {
        hr = fileStream->Read(&grf, sizeof(grf), &cbRead);
    }

    if (SUCCEEDED(hr))
    {
        std::string name = grf.Header.Name;
        if (!name.empty())
        {
            pNodeProperties->SetString(NameProperty, (LPSTR)name.c_str());
        }

        pNodeProperties->SetUInt64(IdProperty, grf.Header.Id);
        pNodeProperties->SetInt32(FormatProperty, (int)grf.Format);
        pNodeProperties->SetInt32(IndicesProperty, (int)grf.IndicesCount);
        pNodeProperties->SetInt32(VerticesProperty, (int)grf.VertexCount);
    }

    pNodeProperties->SetInt32(ContentTypeProperty, (int)GDK::ContentType::ResourceContent);
    pNodeProperties->SetInt32(ContentSubTypeProperty, (int)GDK::ResourceType::GeometryResource);
}

void CContentStudioDoc::WriteContentIndexEntry( IContentStudioProperties* pNodeProperties)
{
    if ((pNodeProperties != nullptr) && (_contentIndex != nullptr))
    {
        std::string id = ReadStringProperty(IdProperty, pNodeProperties);
        if (!id.empty())
        {
            std::string fullPath = ReadStringProperty(FullPathProperty, pNodeProperties);
            std::wstring fullPathW = stde::to_wstring(fullPath); 
            std::string ct = ReadStringProperty(ContentTypeProperty, pNodeProperties);
            std::string cst = ReadStringProperty(ContentSubTypeProperty, pNodeProperties);
            _contentIndex->SetStringValue(id, ct + "," + cst + "," + stde::to_string(FileFromFilePath(fullPathW.c_str(), false)) + "," + stde::to_string(FileFromFilePath(fullPathW.c_str(), true)));
        }
    }
}

void CContentStudioDoc::EnumerateEverything(CFileNode* pRootNode, IContentStudioProperties* pRoot, BOOL skipRootNode)
{
    if (pRootNode != NULL)
    {
        stde::com_ptr<IContentStudioProperties> nodeProps;
        ContentStudioProperties::Create(&nodeProps);
        HRESULT hr = S_OK;

        WriteFileProperties(pRootNode, nodeProps, pRoot);
        WriteGameObjectProperties(pRootNode, nodeProps);
        WriteModelAssetProperties(pRootNode, nodeProps);
        WriteMaterialAssetProperties(pRootNode, nodeProps);
        WriteScreenProperties(pRootNode, nodeProps);
        WriteTextureResourceProperties(pRootNode, nodeProps);
        WriteGeometryResourceProperties(pRootNode, nodeProps);
        WriteVisualComponentProperties(pRootNode, nodeProps);
        WriteLightComponentProperties(pRootNode, nodeProps);
        WriteCameraComponentProperties(pRootNode, nodeProps);

        // Add node to collection
        _documentObjects->Add(nodeProps);

        // Update content index
        WriteContentIndexEntry(nodeProps);

        // Node is a folder and has children, iterate them and recurse
        if (pRootNode->children.size() > 0)
        {
            // Iterate over childern
            for(size_t i = 0; i < pRootNode->children.size(); i++)
            {
                EnumerateEverything(pRootNode->children[i], nodeProps, FALSE);
            }
        }
    }
}

HRESULT CContentStudioDoc::PopulateDocumentObjects()
{
    if (_documentObjects == nullptr)
    {
        return E_UNEXPECTED;
    }

    _documentObjects->Reset();

    HRESULT hr = S_OK;
    std::wstring projectRoot = ReadStringPropertyW(ProjectRootProperty, _documentProperties);

    CFileNode allProjectFiles;
    CFileNode::EnumerateFiles(&allProjectFiles, projectRoot, false);

    stde::com_ptr<IContentStudioProperties> gameProperties;
    ContentStudioProperties::Create(&gameProperties);

    CoreServices::Configuration::Create(&_contentIndex);
    EnumerateEverything(&allProjectFiles, gameProperties, false);
    std::wstring indexPath = ReadStringPropertyW(ContentRepositoryRootProperty, _documentProperties) + L"content.index";
    stde::com_ptr<IStream> spIndex;
    CoreServices::FileStream::Create(indexPath, false, &spIndex);
    _contentIndex->Save(spIndex);
    spIndex = nullptr;
    _contentIndex = nullptr;

    // Mark the document objects property as dirty and notify listeners
    _documentProperties->MarkPropertyChanged(DocumentObjectsProperty);
    _documentProperties->SignalPropertiesChangedEvent();

    return hr;
}

HRESULT CContentStudioDoc::CreateDocument()
{
    HRESULT hr = E_FAIL;
    CFileDialog newProjectFileDialog(FALSE, L"cstudio", GetTitle(), 0, L"Content Studio Files (*.cstudio)|*.cstudio||");
    newProjectFileDialog.GetOFN().lpstrTitle = L"Create a New Game Project";
    if (newProjectFileDialog.DoModal() == IDOK)
    {
        // Clear document contents
        Clear();

        SetTitle(newProjectFileDialog.GetFileTitle());

        std::wstring path = newProjectFileDialog.GetOFN().lpstrFile;
        // Extract full path from .cstudio location and default content respository
        std::wstring rootPath = FilePathOnly(newProjectFileDialog.GetOFN().lpstrFile);
        std::wstring contentRootPath = rootPath + L"ContentRepository\\";

        // Set shared properties
        _documentProperties->SetStringW(ProjectRootProperty, (LPWSTR)rootPath.c_str());
        _documentProperties->SetStringW(ContentRepositoryRootProperty, (LPWSTR)(contentRootPath.c_str()));

        // Create new repository directory
        SHCreateDirectory(nullptr, contentRootPath.c_str());

        hr = SaveDocument(path.c_str(), nullptr);
        if (SUCCEEDED(hr))
        {
            PopulateDocumentObjects();
            _documentProperties->SignalPropertiesChangedEvent();
        }
    }

    return hr;
}

HRESULT CContentStudioDoc::DebugCreateCubeOnPlaneProject(bool useWolfenstein3dMap)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spCubeFileStream;
    stde::com_ptr<IGeometryResource> spCubeResource;
    stde::com_ptr<IStream> spCubeTextureFileStream;
    stde::com_ptr<ITextureResource> spCubeTextureResource;

    std::wstring contentRepositoryPath;
    uint64 cubeGeometryResourceId = 0; // created during resource creation
    uint64 cubeTextureResourceId  = 0; // created during resource creation
    uint64 cubeMaterialAssetId = CoreServices::CreateUniqueId();
    uint64 cubeModelAssetId    = CoreServices::CreateUniqueId();
    uint64 cubeGameObjectId    = CoreServices::CreateUniqueId();
    uint64 visualComponentId   = CoreServices::CreateUniqueId();
    uint64 screenObjectId      = CoreServices::CreateUniqueId();
    uint64 cameraComponentId   = CoreServices::CreateUniqueId();
    uint64 cameraGameObjectId  = CoreServices::CreateUniqueId();
    uint64 lightComponentId    = CoreServices::CreateUniqueId();
    uint64 lightGameObjectId   = CoreServices::CreateUniqueId();

    std::string gameObjectIds = stde::to_string(cubeGameObjectId) + "," + 
                                stde::to_string(cameraGameObjectId) + "," + 
                                stde::to_string(lightGameObjectId);

    GDK::Transform transform;

    CoreServices::ConfigurationPtr cubeMaterialAsset;
    stde::com_ptr<IStream> spCubeMaterialAssetFileStream;
    CoreServices::ConfigurationPtr cubeModelAsset;
    stde::com_ptr<IStream> spCubeModelAssetFileStream;
    CoreServices::ConfigurationPtr cubeGameObject;
    stde::com_ptr<IStream> spCubeGameObjectFileStream;
    CoreServices::ConfigurationPtr cubeVisualComponent;
    stde::com_ptr<IStream> spcubeVisualComponentFileStream;
    CoreServices::ConfigurationPtr screenObject;
    stde::com_ptr<IStream> spScreenObjectFileStream;
    CoreServices::ConfigurationPtr cameraComponent;
    stde::com_ptr<IStream> spCameraComponentFileStream;
    CoreServices::ConfigurationPtr cameraGameObject;
    stde::com_ptr<IStream> spCameraGameObjectFileStream;
    CoreServices::ConfigurationPtr lightGameObject;
    stde::com_ptr<IStream> spLightGameObjectFileStream;
    CoreServices::ConfigurationPtr lightComponent;
    stde::com_ptr<IStream> spLightComponentFileStream;

    contentRepositoryPath = ReadStringPropertyW(ContentRepositoryRootProperty, _documentProperties);

    // Create geometry resource
    if (useWolfenstein3dMap)
    {
        ResourceFactory::CreateW3DE1M1("e1m1", &spCubeResource);
    }
    else
    {
        ResourceFactory::CreateCube("cube", &spCubeResource);
    }
    spCubeResource->GetId(cubeGeometryResourceId);
    CoreServices::FileStream::Create(contentRepositoryPath + stde::to_wstring(cubeGeometryResourceId) + L".geometryresource", false, &spCubeFileStream);
    spCubeResource->Save(spCubeFileStream);
    spCubeFileStream = nullptr;
    spCubeResource = nullptr;

    // Create texture resource
    ResourceFactory::CreateW3DWall("wall", &spCubeTextureResource);
    spCubeTextureResource->GetId(cubeTextureResourceId);
    CoreServices::FileStream::Create(contentRepositoryPath + stde::to_wstring(cubeTextureResourceId) + L".textureresource", false, &spCubeTextureFileStream);
    spCubeTextureResource->Save(spCubeTextureFileStream, TextureResourceSaveFormat::textureresource);
    spCubeTextureFileStream = nullptr;
    spCubeTextureResource = nullptr;

    // Create material asset
    CoreServices::Configuration::Create(&cubeMaterialAsset);
    CoreServices::FileStream::Create(contentRepositoryPath + L"cube.materialasset", false, &spCubeMaterialAssetFileStream);
    cubeMaterialAsset->SetValue<uint64>(IdProperty, cubeMaterialAssetId);
    cubeMaterialAsset->SetStringValue(NameProperty, "Cube Material");
    cubeMaterialAsset->SetValue<int>(MaterialIdProperty, 0);
    cubeMaterialAsset->SetValue<uint64>(DiffuseProperty, cubeTextureResourceId);
    cubeMaterialAsset->Save(spCubeMaterialAssetFileStream);
    spCubeMaterialAssetFileStream = nullptr;
    cubeMaterialAsset = nullptr;

    // Create model asset
    CoreServices::Configuration::Create(&cubeModelAsset);
    CoreServices::FileStream::Create(contentRepositoryPath + L"cube.modelasset", false, &spCubeModelAssetFileStream);
    cubeModelAsset->SetValue<uint64>(IdProperty, cubeModelAssetId);
    cubeModelAsset->SetStringValue(NameProperty, "Cube Model");
    cubeModelAsset->SetStringValue(NumMeshesProperty, "1");
    cubeModelAsset->SetStringValue("Mesh0_Transform", "Identity");
    cubeModelAsset->SetValue<uint64>("Mesh0_Geometry", cubeGeometryResourceId);
    cubeModelAsset->SetValue<uint64>("Mesh0_Material", cubeMaterialAssetId);
    cubeModelAsset->Save(spCubeModelAssetFileStream);
    spCubeModelAssetFileStream = nullptr;
    cubeModelAsset = nullptr;

    // Create visual component
    CoreServices::Configuration::Create(&cubeVisualComponent);
    CoreServices::FileStream::Create(contentRepositoryPath + L"cube.visualcomponent", false, &spcubeVisualComponentFileStream);
    cubeVisualComponent->SetValue<uint64>(IdProperty, visualComponentId);
    cubeVisualComponent->SetStringValue(NameProperty, "Cube Visual Component");
    cubeVisualComponent->SetValue<uint64>(ModelProperty, cubeModelAssetId);
    cubeVisualComponent->Save(spcubeVisualComponentFileStream);
    spcubeVisualComponentFileStream = nullptr;
    cubeVisualComponent = nullptr;

    // Create game object
    transform.Position = DirectX::XMFLOAT3(0, 0, 0);
    transform.Orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
    transform.Scale = DirectX::XMFLOAT3(1, 1, 1);

    CoreServices::Configuration::Create(&cubeGameObject);
    CoreServices::FileStream::Create(contentRepositoryPath + L"cube.gameobject", false, &spCubeGameObjectFileStream);
    cubeGameObject->SetValue<uint64>(IdProperty, cubeGameObjectId);
    cubeGameObject->SetStringValue(NameProperty, "Cube Object");
    cubeGameObject->SetValue<GDK::Transform>(TransformProperty, transform);
    cubeGameObject->SetValue<uint64>(VisualComponentProperty, visualComponentId);
    cubeGameObject->Save(spCubeGameObjectFileStream);
    spCubeGameObjectFileStream = nullptr;
    cubeGameObject = nullptr;

    // Create camera component
    CoreServices::Configuration::Create(&cameraComponent);
    CoreServices::FileStream::Create(contentRepositoryPath + L"camera.cameracomponent", false, &spCameraComponentFileStream);
    cameraComponent->SetValue<uint64>(IdProperty, cameraComponentId);
    cameraComponent->SetStringValue(NameProperty, "Camera Component");
    cameraComponent->SetStringValue(CameraTypeProperty, "Perspective");
    cameraComponent->SetStringValue(FOVProperty, "60");
    cameraComponent->SetStringValue(NearProperty, "0.1");
    cameraComponent->SetStringValue(FarProperty, "100.0");
    cameraComponent->Save(spCameraComponentFileStream);
    spCameraComponentFileStream = nullptr;
    cameraComponent = nullptr;

    // Create camera game object
    transform.Position = DirectX::XMFLOAT3(17, 0, 10);
    transform.Orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
    transform.Scale = DirectX::XMFLOAT3(1, 1, 1);

    CoreServices::Configuration::Create(&cameraGameObject);
    CoreServices::FileStream::Create(contentRepositoryPath + L"camera.gameobject", false, &spCameraGameObjectFileStream);
    cameraGameObject->SetValue<uint64>(IdProperty, cameraGameObjectId);
    cameraGameObject->SetStringValue(NameProperty, "Camera Object");
    cameraGameObject->SetValue<GDK::Transform>(TransformProperty, transform);
    cameraGameObject->SetValue<uint64>(CameraComponentProperty, cameraComponentId);
    cameraGameObject->Save(spCameraGameObjectFileStream);
    spCameraGameObjectFileStream = nullptr;
    cameraGameObject = nullptr;

    // Create light component
    CoreServices::Configuration::Create(&lightComponent);
    CoreServices::FileStream::Create(contentRepositoryPath + L"light.lightcomponent", false, &spLightComponentFileStream);
    lightComponent->SetValue<uint64>(IdProperty, lightComponentId);
    lightComponent->SetStringValue(NameProperty, "Light Component");
    lightComponent->SetStringValue(LightTypeProperty, "Directional");
    lightComponent->SetStringValue(ColorProperty, "1.0, 1.0, 1.0");
    lightComponent->Save(spLightComponentFileStream);
    spLightComponentFileStream = nullptr;
    lightComponent = nullptr;

    // Create light game object
    transform.Position = DirectX::XMFLOAT3(0, 0, 0);
    transform.Scale = DirectX::XMFLOAT3(1, 1, 1);

    DirectX::XMMATRIX lightWorld;
    lightWorld.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
    lightWorld.r[2] = DirectX::XMVectorSet(0, -1, 1, 0); // light direction
    lightWorld.r[0] = DirectX::XMVector3Cross(lightWorld.r[1], lightWorld.r[2]);
    lightWorld.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
    DirectX::XMFLOAT4 orientation;
    DirectX::XMStoreFloat4(&orientation, DirectX::XMQuaternionRotationMatrix(lightWorld));
    transform.Orientation = orientation;

    CoreServices::Configuration::Create(&lightGameObject);
    CoreServices::FileStream::Create(contentRepositoryPath + L"light.gameobject", false, &spLightGameObjectFileStream);
    lightGameObject->SetValue<uint64>(IdProperty, lightGameObjectId);
    lightGameObject->SetStringValue(NameProperty, "Light Object");
    lightGameObject->SetValue<GDK::Transform>(TransformProperty, transform);
    lightGameObject->SetValue<uint64>(LightComponentProperty, lightComponentId);
    lightGameObject->Save(spLightGameObjectFileStream);
    spLightGameObjectFileStream = nullptr;
    lightGameObject = nullptr;

    // Create Screen object
    CoreServices::Configuration::Create(&screenObject);
    CoreServices::FileStream::Create(contentRepositoryPath + L"e1m1.screen", false, &spScreenObjectFileStream);
    screenObject->SetValue<uint64>(IdProperty, screenObjectId);
    screenObject->SetStringValue(NameProperty, "e1m1");
    screenObject->SetStringValue(GameObjectIdsProperty, gameObjectIds);
    screenObject->Save(spScreenObjectFileStream);
    spScreenObjectFileStream = nullptr;
    screenObject = nullptr;
    
    return S_OK;
}

HRESULT CContentStudioDoc::WritePropertiesToConfiguration(CoreServices::ConfigurationPtr spConfiguration)
{
    HRESULT hr = S_OK;
    char tempValue[MAX_PATH] = {0};

    if (SUCCEEDED(_documentProperties->GetString(ProjectRootProperty, tempValue, ARRAYSIZE(tempValue) - 1)))
    {
        spConfiguration->SetStringValue(ProjectRootProperty, tempValue);
    }

    if (SUCCEEDED(_documentProperties->GetString(ContentRepositoryRootProperty, tempValue, ARRAYSIZE(tempValue) - 1)))
    {
        spConfiguration->SetStringValue(ContentRepositoryRootProperty, tempValue);
    }

    if (SUCCEEDED(_documentProperties->GetString(SharedContentRepositoryRootProperty, tempValue, ARRAYSIZE(tempValue) - 1)))
    {
        spConfiguration->SetStringValue(SharedContentRepositoryRootProperty, tempValue);
    }

    return hr;
}

HRESULT CContentStudioDoc::ReadPropertiesFromConfiguration(CoreServices::ConfigurationPtr spConfiguration)
{
    HRESULT hr = S_OK;
    std::string tempValue;

    tempValue = spConfiguration->GetStringValue(ProjectRootProperty);
    _documentProperties->SetString(ProjectRootProperty, (LPSTR)tempValue.c_str());

    tempValue = spConfiguration->GetStringValue(SharedContentRepositoryRootProperty);
    _documentProperties->SetString(SharedContentRepositoryRootProperty, (LPSTR)tempValue.c_str());

    return hr;
}

HRESULT CContentStudioDoc::SaveDocument(LPCWSTR path, HANDLE hFile)
{
    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr spConfiguration;
    stde::com_ptr<IStream> spFileStream;

    hr = CoreServices::Configuration::Create(&spConfiguration);
    if (SUCCEEDED(hr))
    {
        if (hFile)
        {
            hr = CoreServices::FileStream::Attach(hFile, &spFileStream);
        }
        else
        {
            hr = CoreServices::FileStream::Create(path, false, &spFileStream);
        }

        if (SUCCEEDED(hr))
        {
            // write current project settings
            WritePropertiesToConfiguration(spConfiguration);
            hr = spConfiguration->Save(spFileStream);
            SetPathName(path);
        }
    }

    return hr;
}

HRESULT CContentStudioDoc::LoadDocument(LPCWSTR path)
{
    HRESULT hr = S_OK;
    CoreServices::ConfigurationPtr spConfiguration;

    hr = CoreServices::Configuration::Load(path, &spConfiguration);
    if (SUCCEEDED(hr))
    {
        // Read project configuration into shared document properties
        //ReadPropertiesFromConfiguration(spConfiguration);

        // Extract full path from .cstudio location and default content respository
        std::wstring rootPath = FilePathOnly(path);
        std::wstring contentRootPath = rootPath + L"ContentRepository\\";
        std::string finalContentRootPath = stde::to_string(contentRootPath);
        std::string finalProjectRootPath = stde::to_string(rootPath);
        _documentProperties->SetString(ContentRepositoryRootProperty, (LPSTR)finalContentRootPath.c_str());
        _documentProperties->SetString(ProjectRootProperty, (LPSTR)finalProjectRootPath.c_str());

        PopulateDocumentObjects();

        // Signal that the document contents have changed
        _documentProperties->SignalPropertiesChangedEvent();
    }

    return hr;
}

void CContentStudioDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        SaveDocument(ar.m_strFileName.GetString(), ar.GetFile()->m_hFile);
    }
    else
    {
        LoadDocument(ar.m_strFileName.GetString());
    }
}

void CContentStudioDoc::Clear()
{
    _documentProperties->Reset();
    _documentObjects->Reset();
}

// IContentStudioDocument
HRESULT CContentStudioDoc::GetDocumentProperties(IContentStudioProperties** ppProperties)
{
    return _documentProperties->QueryInterface(IID_IUnknown, (void**)ppProperties);
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
