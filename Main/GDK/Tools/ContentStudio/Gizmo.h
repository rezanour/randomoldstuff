#pragma once

enum class EditMode
{
    None,
    Rotate,
    TranslateX,
    TranslateZ,
    TranslateY,
};

class Gizmo
{
#pragma pack(push, 1)
    struct GeometryContentVertex
    {
        GDK::Float3 position;
        GDK::Float3 normal;
        GDK::Float2 texCoord;
    };
#pragma pack(pop)

public:
    Gizmo(_In_ std::shared_ptr<GDK::GraphicsDevice> graphicsDevice);
    ~Gizmo();
    static std::shared_ptr<Gizmo> Create(_In_ std::shared_ptr<GDK::GraphicsDevice> graphicsDevice);

    void Draw(_In_ GDK::Matrix worldMatrix, _In_ Camera& camera, _In_ int viewPortWidth);

    EditMode GetEditMode() const;
    void SetEditMode(_In_ const EditMode editMode);

    bool Pick(_In_ GDK::Ray ray);

private:

    std::shared_ptr<GDK::Geometry> LoadGeometryFromResource(_In_ std::shared_ptr<GDK::GraphicsDevice> graphicsDevice, _In_ UINT resourceId);

    void LoadVertexDataFromBuffer(_In_ std::string textBuffer, std::vector<GeometryContentVertex>& vertices, _Inout_ std::vector<uint32_t>& indices);

    std::shared_ptr<GDK::Texture> LoadTextureFromColor(_In_ std::shared_ptr<GDK::GraphicsDevice> graphicsDevice, float r, float g, float b, float a);

    std::shared_ptr<GDK::GraphicsDevice> _graphicsDevice;
    GDK::Sphere _boundingSphere;

    std::shared_ptr<GDK::Geometry> _rotateY;
    std::shared_ptr<GDK::Geometry> _translateXYZ;
    std::shared_ptr<GDK::CollisionPrimitive> _fastCollision;
    std::shared_ptr<GDK::CollisionPrimitive> _accurateCollision;


    std::shared_ptr<GDK::Texture>  _rotateYTexture;
    std::shared_ptr<GDK::Texture>  _translateXTexture;
    std::shared_ptr<GDK::Texture>  _translateYTexture;
    std::shared_ptr<GDK::Texture>  _translateZTexture;

    GDK::Matrix _rotateYWorld;
    GDK::Matrix _translateXWorld;
    GDK::Matrix _translateYWorld;
    GDK::Matrix _translateZWorld;

    float _desiredProjectedSize;
    EditMode _editMode;
};