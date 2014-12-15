#pragma once

struct IGraphicsSystem;
struct IGraphicsScene;
struct IModel;
struct ILight;
struct PositionNormalTangentTextureVertex;

class GraphicsTestScreen : public BaseObject<GraphicsTestScreen>, public IScreen
{
public:
    static std::shared_ptr<GraphicsTestScreen> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

    //
    // IScreen
    //

    bool SupportsFocus() const override;
    void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) override;
    void Draw(_In_ const DrawContext& context) override;

private:
    GraphicsTestScreen(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

private:
    std::shared_ptr<IGraphicsScene> _scene;
    Transform _camera;
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _projection;
};
