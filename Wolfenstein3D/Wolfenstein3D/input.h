#ifndef _INPUT_H_
#define _INPUT_H_

typedef struct _MOUSE_STATE
{
    POINT pt;
    BOOL buttons[2];
}MOUSE_STATE, *PMOUSE_STATE;

typedef struct _KEYBOARD_STATE
{
    BYTE current[256];
    BYTE previous[256];
}KEYBOARD_STATE, *PKEYBOARD_STATE;

VOID UpdateInput();
BOOL IsKeyDown(BYTE key);
BOOL IsKeyHeld(BYTE key);
BOOL IsKeyJustPressed(BYTE key);
BOOL IsKeyJustReleased(BYTE key);
VOID ClearInputState();

#endif /*_INPUT_H_ */