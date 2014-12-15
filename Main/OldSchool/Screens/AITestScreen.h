#pragma once

struct IGraphicsSystem;
struct IAudioSystem;
struct ISound;
struct ITexture;
struct IGeometry;
struct ISpriteFont;
struct IGraphicsScene;
struct IModel;

class GameWorld;
class Weapon;
class Entity;

enum class EntityClass;
enum class WeaponClass;

class AITestScreen : public BaseObject<AITestScreen>, public IScreen
{
public:
    static std::shared_ptr<AITestScreen> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const std::shared_ptr<IAudioSystem>& audio);

    //
    // IScreen
    //

    bool SupportsFocus() const override;
    void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) override;
    void Draw(_In_ const DrawContext& context) override;

private:
    AITestScreen(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const std::shared_ptr<IAudioSystem>& audio);

    void DrawPanelRect(_In_ const DrawContext& context, _In_ const RECT& rect);
    void DrawStatsPanel(_In_ const DrawContext& context);
    void DrawEntityStats(_In_ const DrawContext& context, _In_ std::shared_ptr<Entity> entity, _In_ uint32_t x, _In_ uint32_t y, bool isTarget);

    void DrawHud(_In_ const DrawContext& context);

    void CreateDebugScene(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);
    void AddBoundingBoxModelForEntity(const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const std::shared_ptr<Entity>& entity);

private:

    std::shared_ptr<GameWorld> _world;
    std::shared_ptr<IGraphicsScene> _debugScene;
    std::vector<std::shared_ptr<IModel>> _debugBoundingBoxModels;
    std::shared_ptr<IGeometry> _bbGeometry;
    std::shared_ptr<ITexture> _bbTexture;

    std::shared_ptr<ISound> _sound;

    std::shared_ptr<ISpriteFont> _spriteFont18pt;
    std::shared_ptr<ISpriteFont> _spriteFont14pt;
    std::shared_ptr<ISpriteFont> _spriteFont48pt;
    std::shared_ptr<ISpriteFont> _spriteFont36pt;

    std::shared_ptr<ITexture> _crosshair;
    std::shared_ptr<ITexture> _transparent;

    float _worldLoadTimer;

    std::shared_ptr<Entity> _observeEntity;
    Transform _observeCamera;
    float _zoomLevel;
    bool _useObserveCamera;
    bool _drawTargetStats;
    bool _drawHud;

    Matrix _view;
    Matrix _projection;

    uint32_t _frames;
    uint32_t _fps;
    float _frameCounterElapsedTime;
};
