#include "stdafx.h"

Gizmo::~Gizmo()
{

}

_Use_decl_annotations_
std::shared_ptr<Gizmo> Gizmo::Create(std::shared_ptr<GDK::GraphicsDevice> graphicsDevice)
{
    return std::make_shared<Gizmo>(graphicsDevice);
}

_Use_decl_annotations_
std::shared_ptr<GDK::Geometry> Gizmo::LoadGeometryFromResource(std::shared_ptr<GDK::GraphicsDevice> graphicsDevice, UINT resourceId)
{
    std::shared_ptr<GDK::GeometryContent> geometryContent;

    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

    std::vector<GeometryContentVertex> allVertices;
    std::vector<uint32_t> allIndices;

    std::unique_ptr<byte_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;

    std::unique_ptr<GDK::GeometryContent::Animation[]> animations;

    // Create animation data (1 frame)
    animations.reset(new GDK::GeometryContent::Animation[1]);
    animations[0].StartFrame = 0;
    animations[0].EndFrame = 0;
    lstrcpyA(animations[0].Name, "gizmo");

    std::string textBuffer;

    // import geometry resource
    HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(resourceId), L"OBJ_GEOMETRY");
    if (hResource)
    {
        HGLOBAL hResourceMemory = LoadResource(NULL, hResource);
        if (hResourceMemory)
        {
            CHAR* pResource = (CHAR*)LockResource(hResourceMemory);
            if (pResource)
            {
                textBuffer = pResource;
            }
        }
    }

    LoadVertexDataFromBuffer(textBuffer, allVertices, allIndices);

    // Create GDK vertices and indices buffers
    vertexData.reset(new byte_t[sizeof(GeometryContentVertex) * allVertices.size()]);
    indexData.reset(new uint32_t[allIndices.size()]);
    memcpy(vertexData.get(), allVertices.data(), sizeof(GeometryContentVertex) * allVertices.size());
    memcpy(indexData.get(), allIndices.data(), sizeof(uint32_t) * allIndices.size());

    // Create geometry
    geometryContent = GDK::GeometryContent::Create(attributeCount, attributes, sizeof(GeometryContentVertex), (uint32_t)allVertices.size(), vertexData, (uint32_t)allIndices.size(), indexData, (uint32_t)1, 1, animations);

    _fastCollision = GDK::Collision::SphereFromGeometry(geometryContent, 0);
    _accurateCollision = GDK::Collision::TriangleMeshFromGeometry(geometryContent, 0);

    return graphicsDevice->CreateGeometry(geometryContent);
}

_Use_decl_annotations_
Gizmo::Gizmo(std::shared_ptr<GDK::GraphicsDevice> graphicsDevice)
{
    _graphicsDevice = graphicsDevice;
    _rotateY = LoadGeometryFromResource(graphicsDevice, IDR_OBJ_ROTATE);
    _translateXYZ = LoadGeometryFromResource(graphicsDevice, IDR_OBJ_TRANSLATE);

    _boundingSphere.radius = 8.0f;
    _desiredProjectedSize = 500.0f;

    _translateXTexture = LoadTextureFromColor(graphicsDevice, 255.0f, 0.0f , 0.0f, 0.0f);
    _translateYTexture = LoadTextureFromColor(graphicsDevice, 0.0f, 255.0f , 0.0f, 0.0f);
    _translateZTexture = LoadTextureFromColor(graphicsDevice, 0.0f, 0.0f , 255.0f, 0.0f);

    _rotateYTexture = LoadTextureFromColor(graphicsDevice, 255.0f, 255.0f , 0.0f, 0.0f);
}

_Use_decl_annotations_
std::shared_ptr<GDK::Texture> Gizmo::LoadTextureFromColor(std::shared_ptr<GDK::GraphicsDevice> graphicsDevice, float r, float g, float b, float a)
{
    std::shared_ptr<GDK::TextureContent> textureContent;

    std::unique_ptr<byte_t[]> pixels;
    std::unique_ptr<GDK::RectangleF[]> frames(new GDK::RectangleF[1]);
    frames.get()[0] = GDK::RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

    pixels.reset(new byte_t[4]);
    pixels.get()[0] = (byte_t)r;
    pixels.get()[1] = (byte_t)g;
    pixels.get()[2] = (byte_t)b;
    pixels.get()[3] = (byte_t)a;

    return graphicsDevice->CreateTexture(GDK::TextureContent::Create(1, 1, GDK::TextureFormat::R8G8B8A8, pixels, 1, frames));
}

_Use_decl_annotations_
void Gizmo::Draw(GDK::Matrix worldMatrix, Camera& camera, int viewPortWidth)
{
    if (_graphicsDevice)
    {
        GDK::Vector3 objectPos = worldMatrix.GetTranslation();

        float scale = GDK::ScaleToDesiredProjectedSize(camera.World().GetTranslation(), 
            camera.World().GetForward(), camera.GetFOV(), (float)viewPortWidth, 
            _boundingSphere.radius, objectPos, _desiredProjectedSize); 

        GDK::Matrix scaleMatrix = GDK::Matrix::CreateScale(GDK::Vector3(scale));

        _graphicsDevice->BindTexture(0, nullptr);

        _rotateYWorld = scaleMatrix * GDK::Matrix::CreateTranslation(worldMatrix.GetTranslation());
        
        _translateXWorld = scaleMatrix * GDK::Matrix::CreateRotationZ(-1.5);
        _translateXWorld.SetTranslation(GDK::Vector3(objectPos.x + _boundingSphere.radius, objectPos.y, objectPos.z));

        _translateYWorld = scaleMatrix * GDK::Matrix::CreateTranslation(GDK::Vector3(objectPos.x, objectPos.y + _boundingSphere.radius, objectPos.z));
        
        _translateZWorld = scaleMatrix * GDK::Matrix::CreateRotationX(1.5);
        _translateZWorld.SetTranslation(GDK::Vector3(objectPos.x, objectPos.y, objectPos.z + _boundingSphere.radius));

        _graphicsDevice->ClearDepth(1.0f);

        if (_editMode == EditMode::Rotate)
        {
            _graphicsDevice->BindGeometry(_rotateY);
            _graphicsDevice->BindTexture(0, _rotateYTexture);
            _graphicsDevice->Draw(_rotateYWorld, 0);
        }
        else
        {
            _graphicsDevice->BindGeometry(_translateXYZ);
            _graphicsDevice->BindTexture(0, _translateXTexture);
            _graphicsDevice->Draw(_translateXWorld, 0);
            _graphicsDevice->BindTexture(0, _translateYTexture);
            _graphicsDevice->Draw(_translateYWorld, 0);
            _graphicsDevice->BindTexture(0, _translateZTexture);
            _graphicsDevice->Draw(_translateZWorld, 0);
        }
    }
}

_Use_decl_annotations_
void Gizmo::LoadVertexDataFromBuffer(std::string textBuffer, std::vector<GeometryContentVertex>& vertices, std::vector<uint32_t>& indices)
{
    std::vector< std::basic_string<char> > x;
    std::split(textBuffer, x, '\n', true);

    std::vector<GDK::Vector3> v;
    std::vector<GDK::Vector3> n;
    std::vector<GDK::Vector2> t;

    for (size_t i = 0; i < x.size(); i++)
    {
        if (x[i].find("v ") == 0)
        {
            GDK::Vector3 vertex;
            sscanf_s(x[i].c_str(), "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            v.push_back(vertex);
        }
        else if (x[i].find("vn ") == 0)
        {
            GDK::Vector3 normal;
            sscanf_s(x[i].c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
            n.push_back(normal);
        }
        else if (x[i].find("vt ") == 0)
        {
            GDK::Vector2 texCoord;
            sscanf_s(x[i].c_str(), "%*s %f %f", &texCoord.x, &texCoord.y);
            t.push_back(texCoord);
        }
        else if (x[i].find("f ") == 0)
        {
            std::vector< std::basic_string<char> > faceVertices;
            std::split(x[i], faceVertices, ' ', true);
            GeometryContentVertex firstFaceVertex;
            for (size_t f = 1 /*skip first*/; f < faceVertices.size(); f++)
            {
                GeometryContentVertex faceVertex;
                int vIndex = 0;
                int tIndex = 0;
                int nIndex = 0;
                std::vector< std::basic_string<char> > vtn;
                std::split(faceVertices[f], vtn, '/', true);
                vIndex = atoi(vtn[0].c_str()) - 1;
                tIndex = atoi(vtn[1].c_str()) - 1;
                nIndex = atoi(vtn[2].c_str()) - 1;

                faceVertex.position.x = v[vIndex].x; faceVertex.position.y = v[vIndex].y; faceVertex.position.z = v[vIndex].z;
                faceVertex.texCoord.x = t[tIndex].x; faceVertex.texCoord.y = t[tIndex].y;
                faceVertex.normal.x = n[nIndex].x; faceVertex.normal.y = n[nIndex].y; faceVertex.normal.z = n[nIndex].z;

                if (f > 3)
                {
                    // triangulate vertex before adding
                    vertices.push_back(firstFaceVertex);
                    vertices.push_back(vertices[vertices.size() - 2]);
                    vertices.push_back(faceVertex);
                }
                else
                {
                    vertices.push_back(faceVertex);
                }

                // cache the first vertex for simplified triangulation
                if (f == 1)
                {
                    firstFaceVertex = faceVertex;
                }
            }
        }
    }

    // Build indices list
    for (uint32_t i = 0; i < (uint32_t)vertices.size(); i++)
    {
        indices.push_back(i);
    }
}

EditMode Gizmo::GetEditMode() const
{
    return _editMode;
}

_Use_decl_annotations_
void Gizmo::SetEditMode(const EditMode editMode)
{
    _editMode = editMode;
}

_Use_decl_annotations_
bool Gizmo::Pick(GDK::Ray ray)
{
    if (GDK::Raycast(&ray, nullptr, _fastCollision.get(), &_translateXWorld, nullptr))
    {
        if (GDK::Raycast(&ray, nullptr, _accurateCollision.get(), &_translateXWorld, nullptr))
        {
            _editMode = EditMode::TranslateX;
            return true;
        }
    }
    else if (GDK::Raycast(&ray, nullptr, _fastCollision.get(), &_translateYWorld, nullptr))
    {
        if (GDK::Raycast(&ray, nullptr, _accurateCollision.get(), &_translateYWorld, nullptr))
        {
            _editMode = EditMode::TranslateY;
            return true;
        }
    }
    else if (GDK::Raycast(&ray, nullptr, _fastCollision.get(), &_translateZWorld, nullptr))
    {
        if (GDK::Raycast(&ray, nullptr, _accurateCollision.get(), &_translateZWorld, nullptr))
        {
            _editMode = EditMode::TranslateZ;
            return true;
        }
    }

    return false;
}