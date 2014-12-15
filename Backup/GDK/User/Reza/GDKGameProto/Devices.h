#pragma once

namespace GDK
{
    enum DeviceType
    {
        DeviceType_Input,
        DeviceType_Renderer,
        DeviceType_Audio,
    };

    enum InputType
    {
        InputType_Keyboard,
        InputType_Mouse,
        InputType_Gamepad,
    };

    // generic device messages for output devices
    enum DeviceMessageType
    {
        DeviceMessage_SessionCreated,
        DeviceMessage_SessionDestroyed,
    };

} // GDK
