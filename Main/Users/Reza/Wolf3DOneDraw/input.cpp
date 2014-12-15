#include "precomp.h"
#include "input.h"

static Input* g_input;

// The following ids extend the original #defines used for input
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

Input::Input()
{
    ZeroMemory(&_gamepadState, sizeof(_gamepadState));
}

void Input::Tick()
{
    InputState inputState;
    if (GetKeyboardState(inputState.KeyboardState))
    {
        UpdateKeyboard(&inputState);
    }

    UpdateController();
}

bool Input::IsMenuUpPressed() const
{
    return (IsKeyJustPressed(VK_UP) || 
            IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_UP) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_UP) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_UP));
}

bool Input::IsMenuSelectPressed() const
{
    return (IsKeyJustPressed(VK_RETURN) || IsControllerButtonJustPressed(INPUT_GAMEPAD_A));
}

bool Input::IsMenuDownPressed() const
{
    return (IsKeyJustPressed(VK_DOWN) || 
            IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_DOWN) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_DOWN) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_DOWN));
}

bool Input::IsMenuRightPressed() const
{
    return (IsKeyJustPressed(VK_RIGHT) || 
            IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_RIGHT) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_RIGHT) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_RIGHT));
}

bool Input::IsMenuLeftPressed() const
{
    return (IsKeyJustPressed(VK_LEFT) || 
            IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_LEFT) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_LEFT) ||
            IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_LEFT));
}

bool Input::IsMenuDismissPressed() const
{
    return (IsKeyJustPressed(VK_ESCAPE) || IsControllerButtonJustPressed(INPUT_GAMEPAD_B));
}

bool Input::IsMenuButtonPressed() const
{
    return (IsKeyJustPressed(VK_ESCAPE) || IsControllerButtonJustPressed(INPUT_GAMEPAD_START));
}

XMFLOAT2 Input::GetLookVector() const
{
    float x = 0.0f;
    float y = 0.0f;

    GetRightThumbStickValue(&x, &y);
    if (IsKeyDown(VK_RIGHT))
    {
        x = 1.0f;
    }
    if (IsKeyDown(VK_LEFT))
    {
        x = -1.0f;
    }
    if (IsKeyDown(VK_UP))
    {
        y = 1.0f;
    }
    if (IsKeyDown(VK_DOWN))
    {
        y = -1.0f;
    }

    return XMFLOAT2(x, y);
}

XMFLOAT2 Input::GetMovementVector() const
{
    float x = 0.0f;
    float y = 0.0f;

    GetLeftThumbStickValue(&x, &y);
    if (IsKeyDown('D'))
    {
        x = 1.0f;
    }
    if (IsKeyDown('A'))
    {
        x = -1.0f;
    }
    if (IsKeyDown('W'))
    {
        y = 1.0f;
    }
    if (IsKeyDown('S'))
    {
        y = -1.0f;
    }

    return XMFLOAT2(x, y);
}

_Use_decl_annotations_
void Input::UpdateKeyboard(InputState* inputState)
{
    memcpy(_keyboardState.Previous, _keyboardState.Current, sizeof(_keyboardState.Current));
    memcpy(_keyboardState.Current, inputState->KeyboardState, sizeof(_keyboardState.Current));
}

void Input::ClearKeyboardState()
{
    ZeroMemory(&_keyboardState, sizeof(_keyboardState));
}

_Use_decl_annotations_
bool Input::IsKeyDown(int16_t key) const
{
    return (_keyboardState.Current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Input::IsKeyHeld(int16_t key) const
{
    return ((_keyboardState.Current[key] & _keyboardState.Previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Input::IsKeyJustPressed(int16_t key) const
{
    return (_keyboardState.Current[key] & 0x80) != 0 && (_keyboardState.Previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Input::IsKeyJustReleased(int16_t key) const
{
    return (_keyboardState.Current[key] & 0x80) == 0 && (_keyboardState.Previous[key] & 0x80) != 0;
}

void Input::UpdateController()
{
    _gamepadState.Previous = _gamepadState.Current;

    XINPUT_STATE state = {};
    if (XInputGetState( 0, &state) == ERROR_DEVICE_NOT_CONNECTED)
    {
        ZeroMemory(&_gamepadState.Current, sizeof(_gamepadState.Current));
        return;
    }

    _gamepadState.Current = state;

    if( (_gamepadState.Current.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        _gamepadState.Current.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (_gamepadState.Current.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        _gamepadState.Current.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
        {
            _gamepadState.Current.Gamepad.sThumbLX = 0;
            _gamepadState.Current.Gamepad.sThumbLY = 0;
        }
    if( (_gamepadState.Current.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
        _gamepadState.Current.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
        (_gamepadState.Current.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
        _gamepadState.Current.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
        {
            _gamepadState.Current.Gamepad.sThumbRX = 0;
            _gamepadState.Current.Gamepad.sThumbRY = 0;
        }
}

_Use_decl_annotations_
bool Input::IsControllerButtonDown(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return !!_gamepadState.Current.Gamepad.bRightTrigger;
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return !!_gamepadState.Current.Gamepad.bLeftTrigger;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return _gamepadState.Current.Gamepad.sThumbLY < 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return _gamepadState.Current.Gamepad.sThumbLY > 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return _gamepadState.Current.Gamepad.sThumbRY < 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return _gamepadState.Current.Gamepad.sThumbRY > 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return _gamepadState.Current.Gamepad.sThumbLX > 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return _gamepadState.Current.Gamepad.sThumbLX < 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return _gamepadState.Current.Gamepad.sThumbRX > 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return _gamepadState.Current.Gamepad.sThumbRX < 0;
    }

    return !!(_gamepadState.Current.Gamepad.wButtons & button);
}

_Use_decl_annotations_
bool Input::IsControllerButtonHeld(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (_gamepadState.Previous.Gamepad.bRightTrigger && _gamepadState.Current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (_gamepadState.Previous.Gamepad.bLeftTrigger && _gamepadState.Current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLY > 0 && _gamepadState.Current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLY > 0 && _gamepadState.Current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRY < 0 && _gamepadState.Current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRY > 0 && _gamepadState.Current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLX > 0 && _gamepadState.Current.Gamepad.sThumbLX > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLX < 0 && _gamepadState.Current.Gamepad.sThumbLX < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRX > 0 && _gamepadState.Current.Gamepad.sThumbRX > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return ( _gamepadState.Previous.Gamepad.sThumbRX < 0 && _gamepadState.Current.Gamepad.sThumbRX < 0);
    }

    return ((_gamepadState.Previous.Gamepad.wButtons & button) && (_gamepadState.Current.Gamepad.wButtons & button));
}

_Use_decl_annotations_
bool Input::IsControllerButtonJustPressed(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (!_gamepadState.Previous.Gamepad.bRightTrigger && _gamepadState.Current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (!_gamepadState.Previous.Gamepad.bLeftTrigger && _gamepadState.Current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbLY < 0) && _gamepadState.Current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbLY > 0) && _gamepadState.Current.Gamepad.sThumbLY > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbRY < 0) && _gamepadState.Current.Gamepad.sThumbRY < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbRY > 0) && _gamepadState.Current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbLX > 0) && _gamepadState.Current.Gamepad.sThumbLX > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbLX < 0) && _gamepadState.Current.Gamepad.sThumbLX < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbRX > 0) && _gamepadState.Current.Gamepad.sThumbRX > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return (!(_gamepadState.Previous.Gamepad.sThumbRX < 0) && _gamepadState.Current.Gamepad.sThumbRX < 0);
    }

    return (((_gamepadState.Previous.Gamepad.wButtons & button) == 0) && (_gamepadState.Current.Gamepad.wButtons & button));
}

_Use_decl_annotations_
bool Input::IsControllerButtonJustReleased(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (_gamepadState.Previous.Gamepad.bRightTrigger && !_gamepadState.Current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (_gamepadState.Previous.Gamepad.bLeftTrigger && !_gamepadState.Current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLY < 0 && !(_gamepadState.Current.Gamepad.sThumbLY < 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLY > 0 && !(_gamepadState.Current.Gamepad.sThumbLY > 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRY < 0 && !(_gamepadState.Current.Gamepad.sThumbRY < 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRY > 0 && !(_gamepadState.Current.Gamepad.sThumbRY > 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLX > 0 && !(_gamepadState.Current.Gamepad.sThumbLX > 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbLX < 0 && !(_gamepadState.Current.Gamepad.sThumbLX < 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (_gamepadState.Previous.Gamepad.sThumbRX > 0 && !(_gamepadState.Current.Gamepad.sThumbRX > 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return ( _gamepadState.Previous.Gamepad.sThumbRX < 0 && !(_gamepadState.Current.Gamepad.sThumbRX < 0));
    }

    return (((_gamepadState.Current.Gamepad.wButtons & button) == 0) && (_gamepadState.Previous.Gamepad.wButtons & button));
}

_Use_decl_annotations_
void Input::GetLeftThumbStickValue(float* x, float* y) const
{
    *x = _gamepadState.Current.Gamepad.sThumbLX / 32768.0f;
    *y = _gamepadState.Current.Gamepad.sThumbLY / 32768.0f;
}

_Use_decl_annotations_
void Input::GetRightThumbStickValue(float* x, float* y) const
{
    *x = _gamepadState.Current.Gamepad.sThumbRX / 32768.0f;
    *y = _gamepadState.Current.Gamepad.sThumbRY / 32768.0f;
}

void InputStartup()
{
    assert(!g_input);

    delete g_input;

    g_input = new Input();
}

void InputShutdown()
{
    assert(g_input);

    delete g_input;
    g_input = nullptr;
}

Input& GetInput()
{
    assert(g_input);
    return *g_input;
}
