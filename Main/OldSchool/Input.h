#pragma once

// TODO: We should make this a proper C++ enum. This isn't a C code base anymore :)
#define INPUT_GAMEPAD_DPAD_UP          0x0001
#define INPUT_GAMEPAD_DPAD_DOWN        0x0002
#define INPUT_GAMEPAD_DPAD_LEFT        0x0004
#define INPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define INPUT_GAMEPAD_START            0x0010
#define INPUT_GAMEPAD_BACK             0x0020
#define INPUT_GAMEPAD_LEFT_THUMB       0x0040
#define INPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define INPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define INPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define INPUT_GAMEPAD_A                0x1000
#define INPUT_GAMEPAD_B                0x2000
#define INPUT_GAMEPAD_X                0x4000
#define INPUT_GAMEPAD_Y                0x8000
#define INPUT_GAMEPAD_RIGHT_TRIGGER     100
#define INPUT_GAMEPAD_LEFT_TRIGGER      101
#define INPUT_GAMEPAD_RIGHT_THUMB_DOWN  102
#define INPUT_GAMEPAD_RIGHT_THUMB_UP    103
#define INPUT_GAMEPAD_RIGHT_THUMB_LEFT  104
#define INPUT_GAMEPAD_RIGHT_THUMB_RIGHT 105
#define INPUT_GAMEPAD_LEFT_THUMB_DOWN   106
#define INPUT_GAMEPAD_LEFT_THUMB_UP     107
#define INPUT_GAMEPAD_LEFT_THUMB_LEFT   108
#define INPUT_GAMEPAD_LEFT_THUMB_RIGHT  109

struct __declspec(novtable) IInput
{
    virtual void Update() = 0;
    virtual bool IsKeyDown(_In_ int16_t key) const = 0;
    virtual bool IsKeyHeld(_In_ int16_t key) const = 0;
    virtual bool IsKeyJustPressed(_In_ int16_t key) const = 0;
    virtual bool IsKeyJustReleased(_In_ int16_t key) const = 0;
    virtual bool IsControllerButtonDown(_In_ int16_t button) const = 0;
    virtual bool IsControllerButtonHeld(_In_ int16_t button) const = 0;
    virtual bool IsControllerButtonJustPressed(_In_ int16_t button) const = 0;
    virtual bool IsControllerButtonJustReleased(_In_ int16_t button) const = 0;
    virtual void GetLeftThumbStickValue(_Out_ float* x, _Out_ float* y) const = 0;
    virtual void GetRightThumbStickValue(_Out_ float* x, _Out_ float* y) const = 0;
};

std::shared_ptr<IInput> CreateInput();
