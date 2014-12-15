#include "common.h"

#define PLATFORM_GDI

typedef struct
{
    USHORT Width;
    USHORT Height;
    HALLOC Alloc;
    BYTE Salt;
} SPRITE;

typedef struct
{
    float   cameraX;
    SHORT   cellX, cellY, stepX, stepY;
    BOOL    enteredFromSide;
    VEC2    position, viewDirection, rayDirection;
    BYTE*   output;
    float*  zBuffer;
    USHORT  outputLength;
} RASTERIZER_PARAMS, *PRASTERIZER_PARAMS;

/* Window and resolution management */
static HWND Window = NULL;
static USHORT ScreenWidth = 0;
static USHORT ScreenHeight = 0;
static RECT Viewport = {0};
/* These are accessed enough that it's worth caching */
static USHORT ViewportWidth = 0;
static USHORT ViewportHeight = 0;
static USHORT HalfViewportWidth = 0;
static USHORT HalfViewportHeight = 0;

/* Palette and drawing */
static W3DCOLOR Palette[256] = {0};
static W3DCOLOR* BackBuffer = NULL;

static BYTE TransparentColor = 0;
static BYTE CeilingColor = 0;
static BYTE FloorColor = 0;

static BYTE* ScratchColumn = NULL;
static float* zBufferColumn = NULL;

/* this is the size of half the perp plane compared to a distance of 1 away from the user.
    In other words, atan(ViewAngleFactor) * 2 == Field of View */
static float ViewAngleFactor = 1.0f; /* FOV of a little over 60 degrees */

/* Sprites */
#define MAX_SPRITES 2000
static SPRITE Sprites[MAX_SPRITES] = {0};
static USHORT NextFree = 0;

/* Platform specific graphics */
#ifdef PLATFORM_GDI

static HDC WindowDC = NULL;             /* Output window to fit our final composited image to. The size of this doesn't have to match our back buffer. We stretch to fit. */
static HDC BackBufferDC = NULL;         /* Resolution-matched buffer where we write our output to as we build it */

#endif

/**************************************************************
    Internal method declarations
***************************************************************/

/* Pixel methods */
VOID W3DSetPixel(USHORT x, USHORT y, BYTE value);
BOOL IsTransparent(BYTE value);

/* Handle methods */
USHORT GetIndexFromHandle(HSPRITE handle);
BYTE GetSaltFromHandle(HSPRITE handle);

/* Sprite methods */
HSPRITE AllocateSprite(USHORT width, USHORT height);
VOID DeleteSprite(USHORT index);
VOID FreeSprite(HSPRITE sprite);

/* Renderer methods */
VOID RasterizeColumn(VEC2 position, VEC2 playerDirection, float cameraX, VEC2 direction, BYTE* output, float* zBuffer, USHORT outputLength);
VOID RasterizeCellContents(PRASTERIZER_PARAMS params);
BOOL RasterizeDoor(PRASTERIZER_PARAMS params);
VOID RasterizeWall(PRASTERIZER_PARAMS params);
VOID FindCellHitLocation(PRASTERIZER_PARAMS params, float* xHit, float* yHit);
VOID RasterizeSpriteColumn(PRASTERIZER_PARAMS params, HSPRITE texture, float xHit, float yHit, float wallX);
VOID RasterizeColumnInternal(BYTE* output, float* zBuffer, USHORT outputLength, SHORT start, SHORT end, float z, BYTE* source, USHORT sourceLength);

/**************************************************************
    Public Graphics API
***************************************************************/
BOOL W3DInitializeGraphics(HWND outputWindow, USHORT width, USHORT height)
{
    USHORT i;
    RECT fullScreen = { 0, 0, width, height };

    /* Ensure we free any resources we are no longer using */
    W3DUninitializeGraphics();

    Window = outputWindow;
    ScreenWidth = width;
    ScreenHeight = height;

    /* Default viewport is full screen */
    W3DSetViewport(&fullScreen);

    /* Fetch the game palette from the data layer */
    GetPalette(Palette);

    /* Find transparent index. The index is normally 255, but we check to be robust against changes. */
    TransparentColor = 255;
    for (i = 0; i < ARRAYSIZE(Palette); ++i)
    {
        if (Palette[i].r == 152 && Palette[i].g == 0 && Palette[i].b == 136)
        {
            TransparentColor = i;
            break;
        }
    }

#ifdef PLATFORM_GDI

    WindowDC = GetDC(outputWindow);
    BackBufferDC = CreateCompatibleDC(WindowDC);

    {
        BITMAPINFO bmi = {0};
        HBITMAP bitmap;

        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = ScreenWidth;
        bmi.bmiHeader.biHeight = -ScreenHeight;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 24;
        bmi.bmiHeader.biCompression = BI_RGB;

        bitmap = CreateDIBSection(BackBufferDC, &bmi, DIB_RGB_COLORS, (PVOID*)&BackBuffer, NULL, 0);
        SelectObject(BackBufferDC, bitmap);
        DeleteObject(bitmap);
    }

#endif /* PLATFORM_GDI */

    return TRUE;
}

VOID W3DUninitializeGraphics()
{
    USHORT i;

    if (zBufferColumn)
    {
        free(zBufferColumn);
        zBufferColumn = NULL;
    }

    if (ScratchColumn)
    {
        free(ScratchColumn);
        ScratchColumn = NULL;
    }

    for (i = 0; i < ARRAYSIZE(Sprites); ++i)
    {
        DeleteSprite(i);
    }

#ifdef PLATFORM_GDI

    if (BackBufferDC)
    {
        DeleteDC(BackBufferDC);
        BackBufferDC = NULL;
    }

    if (WindowDC)
    {
        ReleaseDC(Window, WindowDC);
        WindowDC = NULL;
    }

#endif /* PLATFORM_GDI */
}

USHORT W3DGetScreenWidth()
{
    return ScreenWidth;
}

USHORT W3DGetScreenHeight()
{
    return ScreenHeight;
}

VOID W3DGetViewport(PRECT viewport)
{
    *viewport = Viewport;
}

VOID W3DSetViewport(PRECT viewport)
{
    Viewport = *viewport;

    ViewportWidth = Viewport.right - Viewport.left;
    ViewportHeight = Viewport.bottom - Viewport.top;

    HalfViewportWidth = ViewportWidth / 2;
    HalfViewportHeight = ViewportHeight / 2;

    /* Reset our scratch 'buffers' used during per-column rasterization */

    if (ScratchColumn)
        free(ScratchColumn);

    if (zBufferColumn)
        free(zBufferColumn);

    ScratchColumn = (BYTE*)(malloc)(ViewportHeight);
    zBufferColumn = (float*)malloc(sizeof(float) * ViewportHeight);
}

VOID W3DSetLevelColors(BYTE ceilingColor, BYTE floorColor)
{
    CeilingColor = ceilingColor;
    FloorColor = floorColor;
}

VOID W3DClear(BYTE color)
{
    USHORT x, y;

    for (y = 0; y < ScreenHeight; ++y)
    {
        for (x = 0; x < ScreenWidth; ++x)
        {
            /* Need to use W3DSetPixel so that it maps in the color from the palette */
            W3DSetPixel(x, y, color);
        }
    }
}

VOID W3DPresent()
{
    RECT windowRect;
    GetClientRect(Window, &windowRect);

    StretchBlt(
        WindowDC,
        0, 0,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        BackBufferDC,
        0, 0,
        ScreenWidth,
        ScreenHeight,
        SRCCOPY);
}

VOID W3DDrawLevel(VEC2 playerPosition, VEC2 playerDirection)
{
    USHORT x, y;
    VEC2 dir = Vec2Normalize(playerDirection);
    VEC2 perp = Vec2Scale(Vec2Perp(dir), ViewAngleFactor);
    VEC2 rayDir;
    float cameraX;

    for (x = 0; x < ViewportWidth; ++x)
    {
        cameraX = 2 * x / (float)ViewportWidth - 1;
        rayDir = Vec2Normalize(Vec2Add(dir, Vec2Scale(perp, cameraX)));

        RasterizeColumn(playerPosition, playerDirection, cameraX, rayDir, ScratchColumn, zBufferColumn, ViewportHeight);

        for (y = Viewport.top; y < Viewport.bottom; ++y)
        {
            W3DSetPixel(Viewport.left + x, y, ScratchColumn[y - Viewport.top]);
        }
    }
}

/**************************************************************
    Public Sprite API
***************************************************************/
HSPRITE W3DCreateSpriteFromFile(char* filePath)
{
    HSPRITE sprite = INVALID_HSPRITE_VALUE;
    FILE* bmpFile = NULL;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    BYTE* bits = NULL;

    bmpFile = fopen(filePath, "rb");
    if (bmpFile)
    {
        fread(&bmfh, sizeof(bmfh), 1, bmpFile);
        fread(&bmih, sizeof(bmih), 1, bmpFile);

        if (bmih.biBitCount == 8)
        {
            // skip palette (assumes 256 colors wolf-sized palette
            fseek(bmpFile, sizeof(RGBQUAD) * 256, SEEK_CUR);

            bits = (BYTE*)malloc(bmih.biWidth * bmih.biHeight);
            fread(bits, bmih.biWidth * bmih.biHeight, 1, bmpFile);

            if (bmih.biHeight < 0)
            {
                bmih.biHeight = -bmih.biHeight;
            }

            sprite = AllocateSprite(bmih.biWidth, bmih.biHeight);
            if (W3DIsValidSprite(sprite))
            {
                USHORT x, y;
                PBYTE spriteBits = (PBYTE)MemLock(Sprites[GetIndexFromHandle(sprite)].Alloc);

                for (y = 0; y < bmih.biHeight; ++y)
                {
                    for (x = 0; x < bmih.biWidth; ++x)
                    {
                        spriteBits[x * bmih.biHeight + y] = bits[y * bmih.biWidth + x];
                    }
                }

                MemUnlock(Sprites[GetIndexFromHandle(sprite)].Alloc);
            }

            free(bits);
        }

        fclose(bmpFile);
    }

    return sprite;
}
HSPRITE W3DCreateSprite(USHORT spriteId, BOOL uiSprite)
{
    HSPRITE sprite;
    USHORT i = 0;
    BITMAPFILEHEADER* bmp8bit = NULL;
    BITMAPINFOHEADER* bmp8bitheader = NULL;
    BYTE* bits = NULL;

    if (uiSprite)
    {
        GetUIBitmap(spriteId, &bmp8bit);
    }
    else
    {
        GetSpriteBitmap(spriteId, &bmp8bit);
    }
    
    bits = (BYTE*)bmp8bit;
    bits += sizeof(BITMAPFILEHEADER);

    bmp8bitheader = (BITMAPINFOHEADER*)bits;

    bits += sizeof(BITMAPINFOHEADER);

    bits += sizeof(RGBQUAD) * 256;

    if (bmp8bitheader->biHeight < 0)
    {
        bmp8bitheader->biHeight = -bmp8bitheader->biHeight;
    }

    sprite = AllocateSprite(bmp8bitheader->biWidth, bmp8bitheader->biHeight);
    if (W3DIsValidSprite(sprite))
    {
        USHORT x, y;
        PBYTE spriteBits = (PBYTE)MemLock(Sprites[GetIndexFromHandle(sprite)].Alloc);

        for (y = 0; y < bmp8bitheader->biHeight; ++y)
        {
            for (x = 0; x < bmp8bitheader->biWidth; ++x)
            {
                spriteBits[x * bmp8bitheader->biHeight + y] = bits[y * bmp8bitheader->biWidth + x];
            }
        }

        MemUnlock(Sprites[GetIndexFromHandle(sprite)].Alloc);
    }

    if (bmp8bit)
    {
        free(bmp8bit);
    }

    return sprite;
}

HSPRITE W3DCreateSubSprite(HSPRITE sprite, PRECT source)
{
    SHORT x, y, width, height;
    HSPRITE subsprite;
    SPRITE* src;
    SPRITE* dest;

    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Why you creating a subsprite off of a garbage sprite!?!?");
        return INVALID_HSPRITE_VALUE;
    }

    src = &Sprites[GetIndexFromHandle(sprite)];

    if (source->left < 0 || source->top < 0 ||
        source->right > src->Width || source->bottom > src->Height)
    {
        return INVALID_HSPRITE_VALUE;
    }

    width = source->right - source->left;
    height = source->bottom - source->top;

    subsprite = AllocateSprite(width, height);
    if (W3DIsValidSprite(subsprite))
    {
        PBYTE sourceBits, destBits;

        dest = &Sprites[GetIndexFromHandle(subsprite)];
        sourceBits = (PBYTE)MemLock(src->Alloc);
        destBits = (PBYTE)MemLock(dest->Alloc);

        for (x = source->left; x < source->right; ++x)
        {
            for (y = source->top; y < source->bottom; ++y)
            {
                destBits[(x - source->left) * dest->Height + (y - source->top)] = sourceBits[x * src->Height + y];
            }
        }

        MemUnlock(dest->Alloc);
        MemUnlock(src->Alloc);
    }

    return subsprite;
}

VOID W3DDestroySprite(HSPRITE sprite)
{
    if (W3DIsValidSprite(sprite))
    {
        FreeSprite(sprite);
    }
    else
    {
        assert(FALSE && "Releasing an invalid handle!");
    }
}

BOOL W3DIsValidSprite(HSPRITE sprite)
{
    if (sprite != INVALID_HSPRITE_VALUE && GetIndexFromHandle(sprite) < MAX_SPRITES)
    {
        return GetSaltFromHandle(sprite) == Sprites[GetIndexFromHandle(sprite)].Salt;
    }
    else
    {
        return FALSE;
    }
}

USHORT W3DGetSpriteWidth(HSPRITE sprite)
{
    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Invalid sprite");
        return 0;
    }

    return Sprites[GetIndexFromHandle(sprite)].Width;
}

USHORT W3DGetSpriteHeight(HSPRITE sprite)
{
    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Invalid sprite");
        return 0;
    }

    return Sprites[GetIndexFromHandle(sprite)].Height;
}

VOID W3DDrawSprite2D(HSPRITE sprite, PRECT destination)
{
    SPRITE* s;

    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Trying to draw an invalid sprite!");
        return;
    }

    s = &Sprites[GetIndexFromHandle(sprite)];

    if (s->Alloc != INVALID_HALLOC_VALUE)
    {
        SHORT x, y;
        USHORT imageX, imageY;
        float oneOverWidth = 1.0f / (destination->right - destination->left);
        float oneOverHeight = 1.0f / (destination->bottom - destination->top);
        PBYTE bits = (PBYTE)MemLock(s->Alloc);

        for (y = destination->top; y < destination->bottom; ++y)
        {
            if (y < 0 || y >= ScreenHeight)
                continue;

            for (x = destination->left; x < destination->right; ++x)
            {
                if (x < 0 || x >= ScreenWidth)
                    continue;

                imageX = (x - destination->left) * oneOverWidth * s->Width;
                imageY = (y - destination->top) * oneOverHeight * s->Height;
                W3DSetPixel(x, y, bits[imageX * s->Height + imageY]);
            }
        }

        MemUnlock(s->Alloc);
    }
}

VOID W3DDrawSprite2DSingleColor(HSPRITE sprite, BYTE color, PRECT destination)
{
    SPRITE* s;

    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Invalid sprite");
        return;
    }

    s = &Sprites[GetIndexFromHandle(sprite)];

    if (s->Alloc != INVALID_HALLOC_VALUE)
    {
        SHORT x, y;
        USHORT imageX, imageY;
        float oneOverWidth = 1.0f / (destination->right - destination->left);
        float oneOverHeight = 1.0f / (destination->bottom - destination->top);
        PBYTE bits = (PBYTE)MemLock(s->Alloc);

        for (y = destination->top; y < destination->bottom; ++y)
        {
            if (y < 0 || y >= ScreenHeight)
                continue;

            for (x = destination->left; x < destination->right; ++x)
            {
                if (x < 0 || x >= ScreenWidth)
                    continue;

                imageX = (x - destination->left) * oneOverWidth * s->Width;
                imageY = (y - destination->top) * oneOverHeight * s->Height;
                if (!IsTransparent(bits[imageX * s->Height + imageY]))
                {
                    W3DSetPixel(x, y, color);
                }
            }
        }

        MemUnlock(s->Alloc);
    }
}

/**************************************************************
    Internal methods
***************************************************************/
VOID W3DSetPixel(USHORT x, USHORT y, BYTE value)
{
    if (!IsTransparent(value))
    {
        W3DCOLOR* bb = &BackBuffer[y * ScreenWidth + x];
        W3DCOLOR* p = &Palette[value];

        bb->r = p->b;
        bb->g = p->g;
        bb->b = p->r;
    }
}

BOOL IsTransparent(BYTE value)
{
    return value == TransparentColor;
}

USHORT GetIndexFromHandle(HSPRITE handle)
{
    return handle & 0x0FFF;
}

BYTE GetSaltFromHandle(HSPRITE handle)
{
    return handle >> 12;
}

USHORT MakeHandle(USHORT index)
{
    BYTE salt = ++Sprites[index].Salt;

    assert(GetIndexFromHandle(index) == index);

    if (salt > 15)
    {
        salt = 0;
    }

    Sprites[index].Salt = salt;

    return (((USHORT)salt << 12) | index);
}

HSPRITE AllocateSprite(USHORT width, USHORT height)
{
    if (NextFree < MAX_SPRITES)
    {
        Sprites[NextFree].Alloc = MemAlloc(RESOURCE_TYPE_SPRITE, width * height);
        if (Sprites[NextFree].Alloc != INVALID_HALLOC_VALUE)
        {
            HSPRITE sprite = MakeHandle(NextFree);
            Sprites[NextFree].Width = width;
            Sprites[NextFree].Height = height;

            ++NextFree;
            for (; NextFree < MAX_SPRITES; ++NextFree)
            {
                if (Sprites[NextFree].Alloc == INVALID_HALLOC_VALUE)
                    break;
            }

            return sprite;
        }
    }

        assert(FALSE && "failed to allocate!");
    return INVALID_HSPRITE_VALUE;
}

VOID DeleteSprite(USHORT index)
{
    BYTE salt = Sprites[index].Salt;
    MemFree(Sprites[index].Alloc);
    ZeroMemory(&Sprites[index], sizeof(SPRITE));
    Sprites[index].Salt = salt;

    if (index < NextFree)
    {
        NextFree = index;
    }
}

VOID FreeSprite(HSPRITE sprite)
{
    if (!W3DIsValidSprite(sprite))
    {
        assert(FALSE && "Invalid sprite");
        return;
    }

    DeleteSprite(GetIndexFromHandle(sprite));
}

VOID RasterizeColumn(VEC2 position, VEC2 playerDirection, float cameraX, VEC2 direction, BYTE* output, float* zBuffer, USHORT outputLength)
{
    RASTERIZER_PARAMS params;
    float sideDistX = 0.0f;
    float sideDistY = 0.0f;
    float deltaDistX = sqrt(1 + (direction.y * direction.y) / (direction.x * direction.x));
    float deltaDistY = sqrt(1 + (direction.x * direction.x) / (direction.y * direction.y));
    USHORT i;
    BOOL blocked = FALSE;

    /* initialize params */
    params.cameraX = cameraX;
    params.cellX = position.x;
    params.cellY = position.y;
    params.position = position;
    params.viewDirection = playerDirection;
    params.rayDirection = direction;
    params.output = output;
    params.zBuffer = zBuffer;
    params.outputLength = outputLength;

    for (i = 0; i < outputLength; ++i)
    {
        zBuffer[i] = 1000.0f; /* some large number */
    }

    if (direction.x < 0)
    {
        params.stepX = -1;
        sideDistX = (position.x - params.cellX) * deltaDistX;
    }
    else
    {
        params.stepX = 1;
        sideDistX = (params.cellX + 1.0 - position.x) * deltaDistX;
    }

    if (direction.y < 0)
    {
        params.stepY = -1;
        sideDistY = (position.y - params.cellY) * deltaDistY;
    }
    else
    {
        params.stepY = 1;
        sideDistY = (params.cellY + 1.0 - position.y) * deltaDistY;
    }

    while (!blocked && params.cellX >= 0 && params.cellX < LevelGetWidth() && params.cellY >= 0 && params.cellY < LevelGetHeight())
    {
        params.enteredFromSide = FALSE;

        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            params.cellX += params.stepX;
            params.enteredFromSide = TRUE;
        }
        else
        {
            sideDistY += deltaDistY;
            params.cellY += params.stepY;
        }

        /* if the cell is not a wall, then check for entities and sprites to rasterize first */
        if (!LevelIsCellWall(params.cellX, params.cellY))
        {
            RasterizeCellContents(&params);

            /* if the cell is a door (that's not wide open), we might be occluded by the door so we should check that next */
            if (LevelIsCellDoor(params.cellX, params.cellY) && LevelGetDoorState(params.cellX, params.cellY) != DOOR_STATE_OPEN)
            {
                blocked = RasterizeDoor(&params);
            }

            if (LevelContainsSecret(params.cellX, params.cellY))
            {
                VEC2 secretPos = LevelGetSecret();
                /* TODO: Render Secret walls while in motion */
            }
        }
        else
        {
            /* finally, if we hit a wall, then we can rasterize our wall and complete our rendering */
            RasterizeWall(&params);
            blocked = TRUE;
        }
    }

    /* fill in the rest of the pixels with ceiling and floor color */
    for (i = 0; i < outputLength / 2; ++i)
    {
        if (zBuffer[i] > 999.0f)
        {
            output[i] = CeilingColor;
        }
    }
    for (; i < outputLength; ++i)
    {
        if (zBuffer[i] > 999.0f)
        {
            output[i] = FloorColor;
        }
    }
}

BOOL ComputeSpriteRasterizationInfo(PRASTERIZER_PARAMS params, PVEC2 position, PVEC2 hit, float* imageX)
{
    VEC2 diff = Vec2Add(*position, Vec2Neg(params->position));
    float z = Vec2Dot(diff, params->viewDirection);
    float dist = z / Vec2Dot(params->rayDirection, params->viewDirection);
    *hit = Vec2Add(params->position, Vec2Scale(params->rayDirection, dist));
    diff = Vec2Add(*hit, Vec2Neg(*position));
    dist = Vec2Dot(diff, Vec2Perp(params->viewDirection));

    if (fabs(dist) > 0.5)
        return FALSE;

    *imageX = 0.5 - dist;
    return TRUE;
}

VOID RasterizeCellContents(PRASTERIZER_PARAMS params)
{
    static USHORT indices[10] = {0};
    USHORT i, numIndices;
    HSPRITE sprite;
    VEC2 position, hit;
    float imageX;

    /* Enemies */
    LevelGetEnemies(params->cellX, params->cellY, indices, ARRAYSIZE(indices), &numIndices);
    for (i = 0; i < numIndices; ++i)
    {
        position = EnemyGetPosition(indices[i]);
        if (ComputeSpriteRasterizationInfo(params, &position, &hit, &imageX))
        {
            sprite = EnemyGetSprite(indices[i], params->position, params->viewDirection);
            RasterizeSpriteColumn(params, sprite, hit.x, hit.y, imageX);
        }
    }

    /* Powerups */
    LevelGetPowerups(params->cellX, params->cellY, indices, ARRAYSIZE(indices), &numIndices);
    for (i = 0; i < numIndices; ++i)
    {
        position = PowerupGetPosition(indices[i]);
        if (ComputeSpriteRasterizationInfo(params, &position, &hit, &imageX))
        {
            sprite = PowerupGetSprite(indices[i]);
            RasterizeSpriteColumn(params, sprite, hit.x, hit.y, imageX);
        }
    }

    /* Decor Sprites - NOTE! indices returned here are actually HSPRITES! */
    LevelGetDecorSprites(params->cellX, params->cellY, indices, ARRAYSIZE(indices), &numIndices);
    for (i = 0; i < numIndices; ++i)
    {
        position.x = params->cellX + 0.5f;
        position.y = params->cellY + 0.5f;

        if (ComputeSpriteRasterizationInfo(params, &position, &hit, &imageX))
        {
            RasterizeSpriteColumn(params, indices[i], hit.x, hit.y, imageX);
        }
    }
}

BOOL RasterizeDoor(PRASTERIZER_PARAMS params)
{
    float doorCoverage = 0;
    float actualCoverage = 0;
    float xHit, yHit, wallX;
    BOOL blocked = FALSE;

    FindCellHitLocation(params, &xHit, &yHit);

    /* we need to check to see if we actually hit the door, as it may be partially open */
    if (params->enteredFromSide && LevelIsCellNSDoor(params->cellX, params->cellY))
    {
        /* the door is always halfway in, so advance our hit to there to determine where on the door we hit */
        if (params->stepX > 0)
        {
            xHit += 0.5f;
            yHit += 0.5f * params->rayDirection.y / params->rayDirection.x;
        }
        else
        {
            xHit -= 0.5f;
            yHit -= 0.5f * params->rayDirection.y / params->rayDirection.x;
        }

        /* make sure we didn't leave the cell */
        if (yHit >= params->cellY && yHit <= params->cellY + 1)
        {
            /* compute how far into the door we are, and compare against state */
            doorCoverage = params->cellY + 1 - yHit;
            actualCoverage = LevelGetDoorCoverage(params->cellX, params->cellY);

            if (doorCoverage <= actualCoverage)
            {
                if (actualCoverage - doorCoverage >= 0)
                {
                    wallX = actualCoverage - doorCoverage;
                    if (params->rayDirection.x < 0) wallX = 1 - wallX;
                    blocked = TRUE;
                }
            }
        }
    }
    else if (!params->enteredFromSide && LevelIsCellWEDoor(params->cellX, params->cellY))
    {
        /* the door is always halfway in, so advance our hit to there to determine where on the door we hit */
        if (params->stepY > 0)
        {
            yHit += 0.5f;
            xHit += 0.5f * params->rayDirection.x / params->rayDirection.y;
        }
        else
        {
            yHit -= 0.5f;
            xHit -= 0.5f * params->rayDirection.x / params->rayDirection.y;
        }

        /* make sure we didn't leave the cell */
        if (xHit >= params->cellX && xHit <= params->cellX + 1)
        {
            /* compute how far into the door we are, and compare against state */
            doorCoverage = params->cellX + 1 - xHit;
            actualCoverage = LevelGetDoorCoverage(params->cellX, params->cellY);

            if (doorCoverage <= actualCoverage)
            {
                if (actualCoverage - doorCoverage >= 0)
                {
                    wallX = actualCoverage - doorCoverage;
                    if (params->rayDirection.y > 0) wallX = 1 - wallX;
                    blocked = TRUE;
                }
            }
        }
    }

    if (blocked) /* hit a door */
    {
        RasterizeSpriteColumn(params, LevelGetDoorTexture(params->cellX, params->cellY), xHit, yHit, wallX);
    }

    return blocked;
}

VOID RasterizeWall(PRASTERIZER_PARAMS params)
{
    float xHit, yHit;
    float wallX;
    HSPRITE texture = INVALID_HSPRITE_VALUE;

    FindCellHitLocation(params, &xHit, &yHit);

    if (params->enteredFromSide)
    {
        /* If the cell to the side is a Horizontal door, then we should use the door jam texture */
        if (params->stepX > 0 && LevelIsCellWEDoor(params->cellX - 1, params->cellY))
        {
            texture = LevelGetDoorJamTexture(params->cellX - 1, params->cellY);
        }
        else if (params->stepX < 0 && LevelIsCellWEDoor(params->cellX + 1, params->cellY))
        {
            texture = LevelGetDoorJamTexture(params->cellX + 1, params->cellY);
        }

        wallX = (yHit - params->cellY);
        if (params->rayDirection.x < 0) wallX = 1 - wallX;
    }
    else
    {
        /* If the cell next to us is a Vertical door, then we should use the door jam texture */
        if (params->stepY > 0 && LevelIsCellNSDoor(params->cellX, params->cellY - 1))
        {
            texture = LevelGetDoorJamTexture(params->cellX, params->cellY - 1);
        }
        else if (params->stepY < 0 && LevelIsCellNSDoor(params->cellX, params->cellY + 1))
        {
            texture = LevelGetDoorJamTexture(params->cellX, params->cellY + 1);
        }

        wallX = (xHit - params->cellX);
        if (params->rayDirection.y > 0) wallX = 1 - wallX;
    }

    if (texture == INVALID_HSPRITE_VALUE)
    {
        texture = LevelGetWallTexture(params->cellX, params->cellY);

        if (params->enteredFromSide && LevelWallHasDarkToneAvailable(params->cellX, params->cellY))
        {
            ++texture;
        }
    }

    RasterizeSpriteColumn(params, texture, xHit, yHit, wallX);
}

VOID FindCellHitLocation(PRASTERIZER_PARAMS params, float* xHit, float* yHit)
{
    if (params->enteredFromSide)
    {
        /* since we entered from either left or right side, the xHit must lie on a grid x boundary */
        *xHit = params->cellX + (params->rayDirection.x > 0 ? 0 : 1);

        /* yHit can be computed from the xHit by using the slope */
        *yHit = params->position.y + (*xHit - params->position.x) * (params->rayDirection.y / params->rayDirection.x);
    }
    else
    {
        /* since we entered from either top or bottom, the yHit must lie on a grid y boundary */
        *yHit = params->cellY + (params->rayDirection.y > 0 ? 0 : 1);

        /* xHit can be computed from the yHit by using the slope */
        *xHit = params->position.x + (*yHit - params->position.y) * (params->rayDirection.x / params->rayDirection.y);
    }
}

VOID RasterizeSpriteColumn(PRASTERIZER_PARAMS params, HSPRITE texture, float xHit, float yHit, float wallX)
{
    SHORT lineHeight, startY, endY, textureX;
    SPRITE* s;
    float z;
    PBYTE bits;

    if (!W3DIsValidSprite(texture))
    {
        assert(FALSE && "Invalid sprite");
        return;
    }

    s = &Sprites[GetIndexFromHandle(texture)];
    if (s->Alloc == INVALID_HALLOC_VALUE)
        return;

    bits = (PBYTE)MemLock(s->Alloc);

    /* calculate z value for this column */
    z = params->enteredFromSide ? fabs((xHit - params->position.x) / params->rayDirection.x) : 
                                  fabs((yHit - params->position.y) / params->rayDirection.y);

    z *= Vec2Dot(params->rayDirection, params->viewDirection);

    /* Calculate height of line to draw on screen */
    lineHeight = abs((SHORT)(ViewportHeight / z));
    startY = params->outputLength / 2 - lineHeight * 0.5f;
    endY = startY + lineHeight;

    textureX = wallX * s->Width;
    textureX = max(min(textureX, s->Width - 1), 0);

    RasterizeColumnInternal(params->output, params->zBuffer, params->outputLength, startY, endY, z, bits + (textureX * s->Height), s->Height);

    MemUnlock(s->Alloc);
}

//#define USE_FLOAT_TEXTURE_CALCULATION

VOID RasterizeColumnInternal(BYTE* output, float* zBuffer, USHORT outputLength, SHORT start, SHORT end, float z, BYTE* source, USHORT sourceLength)
{
    USHORT clampedStart = max(start, 0);
    USHORT clampedEnd = min(end, outputLength - 1);
    BYTE color;

#ifndef USE_FLOAT_TEXTURE_CALCULATION
    USHORT i = clampedStart;
    USHORT lineHeight = end - start;
    int d, texY;

    for (; i < clampedEnd; ++i)
    {
        /* Scale up values so we can use integer math, then divide them back down using integers */
        d = i * 256 - outputLength * 128 + lineHeight * 128;
        texY = ((d * sourceLength) / lineHeight) / 256;
        texY = max(min(texY, sourceLength), 0);
        color = source[texY];
        if (z < zBuffer[i] && !IsTransparent(color))
        {
            output[i] = color;
            zBuffer[i] = z;
        }
    }

#else

    USHORT i = clampedStart;
    float scale = sourceLength / (float)(end - start);

    for (; i < clampedEnd; ++i)
    {
        color = source[(USHORT)((i - start) * scale)];
        if (z < zBuffer[i] && !IsTransparent(color))
        {
            output[i] = color;
            zBuffer[i] = z;
        }
    }

#endif
}
