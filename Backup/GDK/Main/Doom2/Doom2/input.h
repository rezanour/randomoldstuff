#ifndef _INPUT_H_
#define _INPUT_H_

BOOL InputInitialize();
void InputUninitialize();
void InputUpdate();
BOOL InputIsKeyDown(short key);
BOOL InputIsKeyHeld(short key);
BOOL InputIsKeyJustPressed(short key);
BOOL InputIsKeyJustReleased(short key);
void InputClearState();

#endif