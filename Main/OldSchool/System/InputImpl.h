#pragma once

#pragma pack(push,1)

typedef struct
{
    uint8_t keyboardState[256];
} inputstate_t;

typedef struct
{
    uint8_t current[256];
    uint8_t previous[256];
} keyboardstate_t;

typedef struct
{
    XINPUT_STATE current;
    XINPUT_STATE previous;
} gamepadstate_t;

#pragma pack(pop)

class Input : public BaseObject<Input>, public IInput
{
public:
    static std::shared_ptr<Input> Create();

    //
    // IInput
    //

    void Update() override;
    bool IsKeyDown(_In_ int16_t key) const override;
    bool IsKeyHeld(_In_ int16_t key) const override;
    bool IsKeyJustPressed(_In_ int16_t key) const override;
    bool IsKeyJustReleased(_In_ int16_t key) const override;
    bool IsControllerButtonDown(_In_ int16_t button) const override;
    bool IsControllerButtonHeld(_In_ int16_t button) const override;
    bool IsControllerButtonJustPressed(_In_ int16_t button) const override;
    bool IsControllerButtonJustReleased(_In_ int16_t button) const override;
    void GetLeftThumbStickValue(_Out_ float* x, _Out_ float* y) const override;
    void GetRightThumbStickValue(_Out_ float* x, _Out_ float* y) const override;

private:
    Input();

    void UpdateKeyboard(_In_ inputstate_t* inputState);
    void ClearKeyboardState();

    void UpdateController();

private:
    keyboardstate_t _keyboardState;
    gamepadstate_t _gamepadState;
};
