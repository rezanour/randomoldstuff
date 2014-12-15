#pragma once

struct IGraphicsSystem;
struct ISpriteFont;

class GameplayScreen : public BaseObject<GameplayScreen>, public IScreen
{
public:
    static std::shared_ptr<GameplayScreen> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

    //
    // IScreen
    //

    bool SupportsFocus() const override;
    void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) override;
    void Draw(_In_ const DrawContext& context) override;

private:
    GameplayScreen(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

private:
    std::shared_ptr<ISpriteFont> _spriteFont;
};
