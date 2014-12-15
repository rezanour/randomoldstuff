#ifndef _W3DGRAPHICS_H_
#define _W3DGRAPHICS_H_

#define HSPRITE                 USHORT
#define INVALID_HSPRITE_VALUE   0xFFFF

typedef struct
{
    BYTE r;
    BYTE g;
    BYTE b;
} W3DCOLOR, *PW3DCOLOR;


/* Graphics API */
BOOL W3DInitializeGraphics(HWND outputWindow, USHORT width, USHORT height);
VOID W3DUninitializeGraphics();

USHORT W3DGetScreenWidth();
USHORT W3DGetScreenHeight();

BOOL W3DIsValidSprite(HSPRITE sprite);
VOID W3DGetViewport(PRECT viewport);
VOID W3DSetViewport(PRECT viewport);

VOID W3DSetLevelColors(BYTE ceilingColor, BYTE floorColor);

VOID W3DClear(BYTE color);
VOID W3DPresent();

VOID W3DDrawLevel(VEC2 playerPosition, VEC2 playerDirection);


/* Sprite API */
HSPRITE W3DCreateSpriteFromFile(char* filePath);
HSPRITE W3DCreateSprite(USHORT spriteId, BOOL uiSprite);
HSPRITE W3DCreateSubSprite(HSPRITE sprite, PRECT source);
VOID W3DDestroySprite(HSPRITE sprite);

USHORT W3DGetSpriteWidth(HSPRITE sprite);
USHORT W3DGetSpriteHeight(HSPRITE sprite);
VOID W3DDrawSprite2D(HSPRITE sprite, PRECT destination);
VOID W3DDrawSprite2DSingleColor(HSPRITE sprite, BYTE color, PRECT destination);

#endif // _W3DGRAPHICS_H_
