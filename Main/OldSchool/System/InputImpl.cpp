#include "Precomp.h"
#include <Xinput.h>
#include "Input.h"
#include "InputImpl.h"

std::shared_ptr<IInput> CreateInput()
{
    return Input::Create();
}

std::shared_ptr<Input> Input::Create()
{
    return std::shared_ptr<Input>(new Input);
}

 Input::Input()
 {
 }

void Input::Update()
{
    inputstate_t inputState;
    if (GetKeyboardState(inputState.keyboardState))
    {
        UpdateKeyboard(&inputState);
    }

    UpdateController();
}

_Use_decl_annotations_
void Input::UpdateKeyboard(inputstate_t* inputState)
{
    memcpy(_keyboardState.previous, _keyboardState.current, sizeof(_keyboardState.current));
    memcpy(_keyboardState.current, inputState->keyboardState, sizeof(_keyboardState.current));
}

void Input::ClearKeyboardState()
{
    ZeroMemory(&_keyboardState, sizeof(_keyboardState));
}

_Use_decl_annotations_
bool Input::IsKeyDown(int16_t key) const
{
    return (_keyboardState.current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Input::IsKeyHeld(int16_t key) const
{
    return ((_keyboardState.current[key] & _keyboardState.previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Input::IsKeyJustPressed(int16_t key) const
{
    return (_keyboardState.current[key] & 0x80) != 0 && (_keyboardState.previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Input::IsKeyJustReleased(int16_t key) const
{
    return (_keyboardState.current[key] & 0x80) == 0 && (_keyboardState.previous[key] & 0x80) != 0;
}

void Input::UpdateController()
{
    _gamepadState.previous = _gamepadState.current;
    XInputGetState( 0, &_gamepadState.current );

    if( (_gamepadState.current.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        _gamepadState.current.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (_gamepadState.current.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        _gamepadState.current.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
        {
            _gamepadState.current.Gamepad.sThumbLX = 0;
            _gamepadState.current.Gamepad.sThumbLY = 0;
        }
    if( (_gamepadState.current.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
        _gamepadState.current.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
        (_gamepadState.current.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
        _gamepadState.current.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
        {
            _gamepadState.current.Gamepad.sThumbRX = 0;
            _gamepadState.current.Gamepad.sThumbRY = 0;
        }
}

_Use_decl_annotations_
bool Input::IsControllerButtonDown(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return !!_gamepadState.current.Gamepad.bRightTrigger;
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return !!_gamepadState.current.Gamepad.bLeftTrigger;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return _gamepadState.current.Gamepad.sThumbLY < 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return _gamepadState.current.Gamepad.sThumbLY > 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return _gamepadState.current.Gamepad.sThumbRY < 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return _gamepadState.current.Gamepad.sThumbRY > 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return _gamepadState.current.Gamepad.sThumbLX > 0;
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return _gamepadState.current.Gamepad.sThumbLX < 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return _gamepadState.current.Gamepad.sThumbRX > 0;
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return _gamepadState.current.Gamepad.sThumbRX < 0;
    }

    return !!(_gamepadState.current.Gamepad.wButtons & button);
}

_Use_decl_annotations_
bool Input::IsControllerButtonHeld(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (_gamepadState.previous.Gamepad.bRightTrigger && _gamepadState.current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (_gamepadState.previous.Gamepad.bLeftTrigger && _gamepadState.current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (_gamepadState.previous.Gamepad.sThumbLY > 0 && _gamepadState.current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (_gamepadState.previous.Gamepad.sThumbLY > 0 && _gamepadState.current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (_gamepadState.previous.Gamepad.sThumbRY < 0 && _gamepadState.current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (_gamepadState.previous.Gamepad.sThumbRY > 0 && _gamepadState.current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (_gamepadState.previous.Gamepad.sThumbLX > 0 && _gamepadState.current.Gamepad.sThumbLX > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (_gamepadState.previous.Gamepad.sThumbLX < 0 && _gamepadState.current.Gamepad.sThumbLX < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (_gamepadState.previous.Gamepad.sThumbRX > 0 && _gamepadState.current.Gamepad.sThumbRX > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return ( _gamepadState.previous.Gamepad.sThumbRX < 0 && _gamepadState.current.Gamepad.sThumbRX < 0);
    }

    return ((_gamepadState.previous.Gamepad.wButtons & button) && (_gamepadState.current.Gamepad.wButtons & button));
}

_Use_decl_annotations_
bool Input::IsControllerButtonJustPressed(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (!_gamepadState.previous.Gamepad.bRightTrigger && _gamepadState.current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (!_gamepadState.previous.Gamepad.bLeftTrigger && _gamepadState.current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbLY < 0) && _gamepadState.current.Gamepad.sThumbLY < 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbLY > 0) && _gamepadState.current.Gamepad.sThumbLY > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbRY < 0) && _gamepadState.current.Gamepad.sThumbRY < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbRY > 0) && _gamepadState.current.Gamepad.sThumbRY > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbLX > 0) && _gamepadState.current.Gamepad.sThumbLX > 0);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbLX < 0) && _gamepadState.current.Gamepad.sThumbLX < 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbRX > 0) && _gamepadState.current.Gamepad.sThumbRX > 0);
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return (!(_gamepadState.previous.Gamepad.sThumbRX < 0) && _gamepadState.current.Gamepad.sThumbRX < 0);
    }

    return (((_gamepadState.previous.Gamepad.wButtons & button) == 0) && (_gamepadState.current.Gamepad.wButtons & button));
}

_Use_decl_annotations_
bool Input::IsControllerButtonJustReleased(int16_t button) const
{
    if (button == INPUT_GAMEPAD_RIGHT_TRIGGER)
    {
        return (_gamepadState.previous.Gamepad.bRightTrigger && !_gamepadState.current.Gamepad.bRightTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_TRIGGER)
    {
        return (_gamepadState.previous.Gamepad.bLeftTrigger && !_gamepadState.current.Gamepad.bLeftTrigger);
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_DOWN)
    {
        return (_gamepadState.previous.Gamepad.sThumbLY < 0 && !(_gamepadState.current.Gamepad.sThumbLY < 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_UP)
    {
        return (_gamepadState.previous.Gamepad.sThumbLY > 0 && !(_gamepadState.current.Gamepad.sThumbLY > 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_DOWN)
    {
        return (_gamepadState.previous.Gamepad.sThumbRY < 0 && !(_gamepadState.current.Gamepad.sThumbRY < 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_UP)
    {
        return (_gamepadState.previous.Gamepad.sThumbRY > 0 && !(_gamepadState.current.Gamepad.sThumbRY > 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_RIGHT)
    {
        return (_gamepadState.previous.Gamepad.sThumbLX > 0 && !(_gamepadState.current.Gamepad.sThumbLX > 0));
    }

    if (button == INPUT_GAMEPAD_LEFT_THUMB_LEFT)
    {
        return (_gamepadState.previous.Gamepad.sThumbLX < 0 && !(_gamepadState.current.Gamepad.sThumbLX < 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_LEFT)
    {
        return (_gamepadState.previous.Gamepad.sThumbRX > 0 && !(_gamepadState.current.Gamepad.sThumbRX > 0));
    }

    if (button == INPUT_GAMEPAD_RIGHT_THUMB_RIGHT)
    {
        return ( _gamepadState.previous.Gamepad.sThumbRX < 0 && !(_gamepadState.current.Gamepad.sThumbRX < 0));
    }

    return (((_gamepadState.current.Gamepad.wButtons & button) == 0) && (_gamepadState.previous.Gamepad.wButtons & button));
}

_Use_decl_annotations_
void Input::GetLeftThumbStickValue(float* x, float* y) const
{
    *x = _gamepadState.current.Gamepad.sThumbLX / 32768.0f;
    *y = _gamepadState.current.Gamepad.sThumbLY / 32768.0f;
}

_Use_decl_annotations_
void Input::GetRightThumbStickValue(float* x, float* y) const
{
    *x = _gamepadState.current.Gamepad.sThumbRX / 32768.0f;
    *y = _gamepadState.current.Gamepad.sThumbRY / 32768.0f;
}
