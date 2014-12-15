#pragma once

struct IGraphicsSystem;
struct ISpriteFont;

class TitleScreen : public BaseObject<TitleScreen>, public IScreen
{
public:
    static std::shared_ptr<TitleScreen> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

    //
    // IScreen
    //

    bool SupportsFocus() const override;
    void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) override;
    void Draw(_In_ const DrawContext& context) override;

private:
    TitleScreen(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

private:
    float _timer;
    std::shared_ptr<ISpriteFont> _spriteFont;
};
