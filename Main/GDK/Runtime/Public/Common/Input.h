#pragma once

#include <Platform.h>
#include <Vector2.h>

namespace GDK
{
    enum class Button
    {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        Up, Down, Left, Right,
        Space, Enter, Escape, Tab, Shift, Control, Alt, Meta,
        MouseLeft, MouseMiddle, MouseRight,

        COUNT
    };

    namespace Input
    {
        void BindButton(_In_ int32_t id, _In_ Button button);
        bool IsButtonDown(_In_ int32_t id);
        bool WasButtonPressed(_In_ int32_t id);
        bool WasButtonReleased(_In_ int32_t id);
        
        Vector2 GetMousePosition();
        Vector2 GetMouseDelta();

        void LockMouseCursor();
        void UnlockMouseCursor();
        
        Button GetButtonByName(_In_ const std::wstring& name);
        std::wstring GetButtonName(_In_ Button button);

        // Template versions of all the ID methods so games can have their own enums for input buttons and not have to manually case to int32_t                
        template <typename T> void BindButton(_In_ T id, _In_ Button button) { BindButton(static_cast<int32_t>(id), button); }        
        template <typename T> bool IsButtonDown(_In_ T id) { return IsButtonDown(static_cast<int32_t>(id)); }        
        template <typename T> bool WasButtonPressed(_In_ T id) { return WasButtonPressed(static_cast<int32_t>(id)); }        
        template <typename T> bool WasButtonReleased(_In_ T id) { return WasButtonReleased(static_cast<int32_t>(id)); }
    }
}