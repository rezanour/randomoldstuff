#ifndef _W3DDATA_H_
#define _W3DDATA_H_

VOID GetPalette(W3DCOLOR palette[]);

/* These will need to chage to meet the game's design, for now, they will return proper 8-bit bitmap files */
BOOL GetUIBitmap(int id, BITMAPFILEHEADER** ppBitmap);
BOOL GetSpriteBitmap(int id, BITMAPFILEHEADER** ppBitmap);

BOOL GetLayerData(BYTE levelNumber, USHORT** ppLayer);
BOOL GetLayerDataEx(BYTE layerId, BYTE levelNumber, USHORT** ppLayer);

VOID GetCeilingColor(BYTE levelNumber, BYTE* ceiling, BYTE* floor);

VOID RectFromCharacter(char ch, RECT* rect);

#endif