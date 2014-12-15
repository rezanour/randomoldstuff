#include <Input.h>
#include <InputHost.h>
#include <InputInternal.h>
#include <GDKError.h>
#include <map>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

using namespace GDK;

// Mapping buttons to strings and back
static std::map<std::wstring, Button> g_nameToButtonMap;
static std::map<Button, std::wstring> g_buttonToNameMap;

// Game defined map filled through the BindButton method
static std::map<int32_t, Button> g_buttonIdLookup;

// Current and previous states for polling, plus the next button state that takes effect on the next Tick()
static std::array<bool, (int)Button::COUNT> g_currentButtonState;
static std::array<bool, (int)Button::COUNT> g_previousButtonState;
static std::array<bool, (int)Button::COUNT> g_nextButtonState;

// Whether or not to lock the mouse position
static bool g_lockMousePos = false;

// Current and previous states for polling, plus the next button state that takes effect on the next Tick()
static Vector2 g_currentMousePos = Vector2();
static Vector2 g_previousMousePos = Vector2();
static Vector2 g_mouseDelta = Vector2();

#ifdef WIN32
static HWND g_hwnd = nullptr;
#endif

_Use_decl_annotations_
Button GDK::Input::GetButtonByName(const std::wstring& name)
{
    return g_nameToButtonMap[name];
}

_Use_decl_annotations_
std::wstring GDK::Input::GetButtonName(Button button)
{
    return g_buttonToNameMap[button];
}

_Use_decl_annotations_
void GDK::Input::BindButton(int32_t id, Button button)
{
    g_buttonIdLookup[id] = button;
}

_Use_decl_annotations_
bool GDK::Input::IsButtonDown(int32_t id)
{
    return g_currentButtonState[(int)g_buttonIdLookup[id]];
}

_Use_decl_annotations_
bool GDK::Input::WasButtonPressed(int32_t id)
{
    return g_currentButtonState[(int)g_buttonIdLookup[id]] && !g_previousButtonState[(int)g_buttonIdLookup[id]];
}

_Use_decl_annotations_
bool GDK::Input::WasButtonReleased(int32_t id)
{
    return !g_currentButtonState[(int)g_buttonIdLookup[id]] && g_previousButtonState[(int)g_buttonIdLookup[id]];
}

Vector2 GDK::Input::GetMousePosition()
{
    return g_lockMousePos ? Vector2() : g_currentMousePos;
}

Vector2 GDK::Input::GetMouseDelta()
{
    return g_mouseDelta;
}

void GDK::Input::LockMouseCursor()
{
    g_lockMousePos = true;
}

void GDK::Input::UnlockMouseCursor()
{
    g_lockMousePos = false;
}

_Use_decl_annotations_
void GDK::Input::OnButtonDown(Button button)
{
    g_nextButtonState[(int)button] = true;
}

_Use_decl_annotations_
void GDK::Input::OnButtonUp(Button button)
{
    g_nextButtonState[(int)button] = false;
}

void GDK::Input::Tick()
{
    for (auto i = 0; i < g_currentButtonState.size(); i++)
    {
        g_previousButtonState[i] = g_currentButtonState[i];
        g_currentButtonState[i] = g_nextButtonState[i];
    }

    g_previousMousePos = g_currentMousePos;

#ifdef WIN32
    POINT point;
    GetCursorPos(&point);
    g_mouseDelta = Vector2(point.x, point.y) - g_previousMousePos;

    if (g_lockMousePos)
    {
        RECT rect;
        GetWindowRect(g_hwnd, &rect);
        // We don't want to set to 0, which is the topleft corner.
        // That would prevent left and up movement. Instead, we pick
        // a value somewhere in the window that gives us enough room
        // on each side.
        SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
    }

    GetCursorPos(&point);
    g_currentMousePos = Vector2(point.x, point.y);
#endif
}

#ifdef WIN32
_Use_decl_annotations_
void GDK::Input::SetWindow(void* hwnd)
{
    g_hwnd = reinterpret_cast<HWND>(hwnd);
}
#endif

void GDK::Input::Init()
{
    int i = 0;

// Handles registering the button in the string tables and initializes the button state arrays to ensure they have proper default values
#define INIT_BUTTON(i, b) { \
    auto str = WIDEN(#b); \
    g_nameToButtonMap[str] = Button::b; \
    g_buttonToNameMap[Button::b] = str; \
    g_currentButtonState[(int)Button::b] = false; \
    g_previousButtonState[(int)Button::b] = false; \
    g_nextButtonState[(int)Button::b] = false; \
    i++; \
}

    INIT_BUTTON(i, A);
    INIT_BUTTON(i, B);
    INIT_BUTTON(i, C);
    INIT_BUTTON(i, D);
    INIT_BUTTON(i, E);
    INIT_BUTTON(i, F);
    INIT_BUTTON(i, G);
    INIT_BUTTON(i, H);
    INIT_BUTTON(i, I);
    INIT_BUTTON(i, J);
    INIT_BUTTON(i, K);
    INIT_BUTTON(i, L);
    INIT_BUTTON(i, M);
    INIT_BUTTON(i, N);
    INIT_BUTTON(i, O);
    INIT_BUTTON(i, P);
    INIT_BUTTON(i, Q);
    INIT_BUTTON(i, R);
    INIT_BUTTON(i, S);
    INIT_BUTTON(i, T);
    INIT_BUTTON(i, U);
    INIT_BUTTON(i, V);
    INIT_BUTTON(i, W);
    INIT_BUTTON(i, X);
    INIT_BUTTON(i, Y);
    INIT_BUTTON(i, Z);
    INIT_BUTTON(i, D0);
    INIT_BUTTON(i, D1);
    INIT_BUTTON(i, D2);
    INIT_BUTTON(i, D3);
    INIT_BUTTON(i, D4);
    INIT_BUTTON(i, D5);
    INIT_BUTTON(i, D6);
    INIT_BUTTON(i, D7);
    INIT_BUTTON(i, D8);
    INIT_BUTTON(i, D9);
    INIT_BUTTON(i, Up);
    INIT_BUTTON(i, Down);
    INIT_BUTTON(i, Left);
    INIT_BUTTON(i, Right);
    INIT_BUTTON(i, Space);
    INIT_BUTTON(i, Enter);
    INIT_BUTTON(i, Escape);
    INIT_BUTTON(i, Tab);
    INIT_BUTTON(i, Shift);
    INIT_BUTTON(i, Control);
    INIT_BUTTON(i, Alt);
    INIT_BUTTON(i, Meta);
    INIT_BUTTON(i, MouseLeft);
    INIT_BUTTON(i, MouseMiddle);
    INIT_BUTTON(i, MouseRight);
    CHECK_TRUE(i == (int)Button::COUNT);

#undef INIT_BUTTON

}

#ifdef WIN32
_Use_decl_annotations_
bool GDK::Input::GetButtonFromWin32Keycode(byte_t keycode, Button* outButton)
{
    switch (keycode)
    {
        case 0x01: *outButton = Button::MouseLeft; return true;
        case 0x02: *outButton = Button::MouseRight; return true;
        case 0x04: *outButton = Button::MouseMiddle; return true;
        case 0x09: *outButton = Button::Tab; return true;
        case 0x0D: *outButton = Button::Enter; return true;
        case 0x10: *outButton = Button::Shift; return true;
        case 0x11: *outButton = Button::Control; return true;
        case 0x12: *outButton = Button::Alt; return true;
        case 0x1B: *outButton = Button::Escape; return true;
        case 0x20: *outButton = Button::Space; return true;
        case 0x25: *outButton = Button::Left; return true;
        case 0x26: *outButton = Button::Up; return true;
        case 0x27: *outButton = Button::Right; return true;
        case 0x28: *outButton = Button::Down; return true;
        case 0x30: *outButton = Button::D0; return true;
        case 0x31: *outButton = Button::D1; return true;
        case 0x32: *outButton = Button::D2; return true;
        case 0x33: *outButton = Button::D3; return true;
        case 0x34: *outButton = Button::D4; return true;
        case 0x35: *outButton = Button::D5; return true;
        case 0x36: *outButton = Button::D6; return true;
        case 0x37: *outButton = Button::D7; return true;
        case 0x38: *outButton = Button::D8; return true;
        case 0x39: *outButton = Button::D9; return true;
        case 0x41: *outButton = Button::A; return true;
        case 0x42: *outButton = Button::B; return true;
        case 0x43: *outButton = Button::C; return true;
        case 0x44: *outButton = Button::D; return true;
        case 0x45: *outButton = Button::E; return true;
        case 0x46: *outButton = Button::F; return true;
        case 0x47: *outButton = Button::G; return true;
        case 0x48: *outButton = Button::H; return true;
        case 0x49: *outButton = Button::I; return true;
        case 0x4A: *outButton = Button::J; return true;
        case 0x4B: *outButton = Button::K; return true;
        case 0x4C: *outButton = Button::L; return true;
        case 0x4D: *outButton = Button::M; return true;
        case 0x4E: *outButton = Button::N; return true;
        case 0x4F: *outButton = Button::O; return true;
        case 0x50: *outButton = Button::P; return true;
        case 0x51: *outButton = Button::Q; return true;
        case 0x52: *outButton = Button::R; return true;
        case 0x53: *outButton = Button::S; return true;
        case 0x54: *outButton = Button::T; return true;
        case 0x55: *outButton = Button::U; return true;
        case 0x56: *outButton = Button::V; return true;
        case 0x57: *outButton = Button::W; return true;
        case 0x58: *outButton = Button::X; return true;
        case 0x59: *outButton = Button::Y; return true;
        case 0x5A: *outButton = Button::Z; return true;
        default: *outButton = Button::COUNT; return false;
    }
}
#endif