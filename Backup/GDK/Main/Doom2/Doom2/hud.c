#include "common.h"

#define DUDEFACE_ANIMATION_INTERVAL     0.7f
FLOAT g_faceAnimationTime = 0;

#define MESSAGE_CLEAR_INTERVAL  3.0f
FLOAT g_messageClearTime = 0;

#define GUN_BOB_ANIMATION_INTERVAL      0.1f
FLOAT g_gunBobAnimationTime = 0;

FLOAT g_gunFireAnimationTime = 0;

#define WEAPON_IDLE_FRAME   0

int g_gunBobY = 0;
int g_gunBobX = 0;
BOOL g_gunBobDir = TRUE;
BOOL g_gunBobDirX = TRUE;

BOOL g_gunFiring = FALSE;

char g_hudMessageText[80];
char g_hudWeaponFrameImage[64];

resourcemanager_t g_HudResourceManager;

resource_t HudGetSprite(const char* name);

#define DIGIT_TERMINATOR  255
VOID DrawDigits(int x, int y, byte_t* digits, int count, BOOL smallDigits);
VOID NumberToDigitArray(INT value, BYTE* digits, INT count);

#define FACE_ANIMATION_COUNT 3 // a count of 5 includes side face turns
#define FACE0_START_INDEX   0
#define FACE1_START_INDEX   8
#define FACE2_START_INDEX   16
#define FACE3_START_INDEX   24
#define FACE4_START_INDEX   32

#define FACE_START_INDEX_OFFSET 8

#define FACE_GODMODE        40
#define FACE_DEAD           41

byte_t g_faceId = 0;
char* g_faces[] = 
{
    "STFST00",
    "STFST01",
    "STFST02",
    "STFTL00",
    "STFTR00",
    "STFOUCH0",
    "STFEVL0",
    "STFKILL0",

    "STFST10",
    "STFST11",
    "STFST12",
    "STFTL10",
    "STFTR10",
    "STFOUCH1",
    "STFEVL1",
    "STFKILL1",

    "STFST20",
    "STFST21",
    "STFST22",
    "STFTL20",
    "STFTR20",
    "STFOUCH2",
    "STFEVL2",
    "STFKILL2",

    "STFST30",
    "STFST31",
    "STFST32",
    "STFTL30",
    "STFTR30",
    "STFOUCH3",
    "STFEVL3",
    "STFKILL3",

    "STFST40",
    "STFST41",
    "STFST42",
    "STFTL40",
    "STFTR40",
    "STFOUCH4",
    "STFEVL4",
    "STFKILL4",

    "STFGOD0", 
    "STFDEAD0",
};

typedef struct 
{
    char* Frame;
    char* Overlay;
    BOOL PlaySound;
} weapon_animation_frame_t;

weapon_animation_frame_t g_fistFrames[] = 
{
    {"PUNGA0", NULL, FALSE}, // WEAPON_IDLE_FRAME
    {"PUNGB0", NULL, FALSE},
    {"PUNGC0", NULL, FALSE},
    {"PUNGD0", NULL, TRUE},
};

weapon_animation_frame_t g_pistolFrams[] = 
{
    {"PISGA0", NULL,     FALSE},
    {"PISGB0", NULL,     FALSE},
    {"PISGC0", NULL,     FALSE},
    {"PISGD0", "PISFA0", TRUE},
    {"PISGE0", NULL,     FALSE},
};

weapon_animation_frame_t g_shotgunFrames[] = 
{
    {"SHTGA0", NULL,     FALSE},
    {"SHTGA0", "SHTFA0", FALSE},
    {"SHTGA0", "SHTFB0", FALSE},
    {"SHTGB0", NULL, FALSE},
    {"SHTGC0", NULL,     FALSE},
    {"SHTGD0", NULL,     FALSE},
};

weapon_animation_frame_t g_chaingunFrames[] = 
{
    {"CHGGA0", NULL,     FALSE},
    {"CHGGB0", "CHGFB0", FALSE},
    {"CHGGA0", "CHGFA0", FALSE},
    {"CHGGB0", "CHGFB0", FALSE},
};

weapon_animation_frame_t g_missileFrames[] = 
{
    {"MISGA0", NULL,     FALSE},
    {"MISGA0", "MISFA0", FALSE},
    {"MISGB0", "MISFB0", FALSE},
    {"MISGB0", "MISFC0", FALSE},
    {"MISGB0", "MISFD0", FALSE},
};

weapon_animation_frame_t g_plasmaFrames[] = 
{
    {"PLSGA0", NULL,     FALSE},
    {"PLSGA0", "PLSFA0", FALSE},
    {"PLSGA0", "PLSFB0", FALSE},
    //{"PLSGB0", NULL, FALSE}, // (looks silly)
};

weapon_animation_frame_t g_bfgFrames[] = 
{
    {"BFGGA0", NULL,     FALSE},
    {"BFGGB0", NULL,     FALSE},
    {"BFGGC0", "BFGFA0", FALSE},
    {"BFGGC0", "BFGFB0", FALSE},
};

weapon_animation_frame_t g_supershotgunFrames[] = 
{
    {"SHT2A0", NULL,     FALSE},
};

weapon_animation_frame_t g_chainsawFrames[] = 
{
    {"SAWGA0", NULL,     FALSE},
};

typedef struct
{
    weapon_type_t Type;
    int           FrameCount;
    weapon_animation_frame_t*        Frames;
    float         TimeBetweenFrame;
    int           CurrentFrame;
    int           FireFrame;
}weapon_animation_info_t;

weapon_animation_info_t g_weaponFrames[] = 
{
    {weapon_type_fist,         ARRAYSIZE(g_fistFrames),         g_fistFrames,         .15f, WEAPON_IDLE_FRAME, 3},
    {weapon_type_pistol,       ARRAYSIZE(g_pistolFrams),        g_pistolFrams,        .15f, WEAPON_IDLE_FRAME, 3},
    {weapon_type_shotgun,      ARRAYSIZE(g_shotgunFrames),      g_shotgunFrames,      .15f, WEAPON_IDLE_FRAME, 2},
    {weapon_type_chaingun,     ARRAYSIZE(g_chaingunFrames),     g_chaingunFrames,     .01f, WEAPON_IDLE_FRAME, 3},
    {weapon_type_missile,      ARRAYSIZE(g_missileFrames),      g_missileFrames,      .15f, WEAPON_IDLE_FRAME, 4},
    {weapon_type_plasma,       ARRAYSIZE(g_plasmaFrames),       g_plasmaFrames,       .15f, WEAPON_IDLE_FRAME, 2},
    {weapon_type_bfg,          ARRAYSIZE(g_bfgFrames),          g_bfgFrames,          .15f, WEAPON_IDLE_FRAME, 3},
    {weapon_type_chainsaw,     ARRAYSIZE(g_chainsawFrames),     g_chainsawFrames,     .15f, WEAPON_IDLE_FRAME, 0},
    {weapon_type_supershotgun, ARRAYSIZE(g_supershotgunFrames), g_supershotgunFrames, .15f, WEAPON_IDLE_FRAME, 0},
};

BOOL HudCreate()
{
    g_HudResourceManager = ResourceManagerCreate();
    return (g_HudResourceManager ? TRUE : FALSE);
}

void HudDestroy()
{
    if (g_HudResourceManager)
    {
        ResourceManagerDestroy(g_HudResourceManager);
    }
}

void HudRenderAnimatedFace(int playerHealth)
{
    // Handle face animation timer
    g_faceAnimationTime += GameTimeGetElapsed();
    if (g_faceAnimationTime > DUDEFACE_ANIMATION_INTERVAL)
    {
        g_faceId =  rand() % FACE_ANIMATION_COUNT;

        if (playerHealth < 90) { g_faceId += FACE_START_INDEX_OFFSET; } // 1
        if (playerHealth < 70) { g_faceId += FACE_START_INDEX_OFFSET; } // 2
        if (playerHealth < 50) { g_faceId += FACE_START_INDEX_OFFSET; } // 3
        if (playerHealth < 30) { g_faceId += FACE_START_INDEX_OFFSET; } // 4

        if (playerHealth == 0)
        {
            g_faceId = FACE_DEAD;
        }

        if (PlayerInGodMode())
        {
            g_faceId = FACE_GODMODE;
        }

        g_faceAnimationTime = 0;
    }

    GraphicsDrawSprite(144,168, HudGetSprite(g_faces[g_faceId]));
}

void HudRenderHealth(int playerHealth)
{
    byte_t digitArray[8];
    NumberToDigitArray(playerHealth, digitArray, ARRAYSIZE(digitArray));
    DrawDigits(77,171, digitArray, ARRAYSIZE(digitArray), FALSE);
    GraphicsDrawSprite(90,171, HudGetSprite("STTPRCNT"));
}

void HudRenderAmmo(int playerAmmo)
{
    byte_t digitArray[8];
    NumberToDigitArray(playerAmmo, digitArray, ARRAYSIZE(digitArray));
    DrawDigits(30,171, digitArray, ARRAYSIZE(digitArray), FALSE);
}

void HudRenderArmor(int playerArmor)
{
    byte_t digitArray[8];
    NumberToDigitArray(playerArmor, digitArray, ARRAYSIZE(digitArray));
    DrawDigits(208,171, digitArray, ARRAYSIZE(digitArray), FALSE);
    GraphicsDrawSprite(221,171, HudGetSprite("STTPRCNT"));
}

void HudRenderAmmoClipData()
{
    byte_t digitArray[8];

    // bullets
    NumberToDigitArray(PlayerGetAmmo(weapon_ammo_bullets), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(283,173, digitArray, ARRAYSIZE(digitArray), TRUE);

    NumberToDigitArray(PlayerGetMaxAmmo(weapon_ammo_bullets), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(310,173, digitArray, ARRAYSIZE(digitArray), TRUE);

    // shells
    NumberToDigitArray(PlayerGetAmmo(weapon_ammo_shells), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(283,179, digitArray, ARRAYSIZE(digitArray), TRUE);

    NumberToDigitArray(PlayerGetMaxAmmo(weapon_ammo_shells), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(310,179, digitArray, ARRAYSIZE(digitArray), TRUE);

    // rockets
    NumberToDigitArray(PlayerGetAmmo(weapon_ammo_rockets), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(283,185, digitArray, ARRAYSIZE(digitArray), TRUE);

    NumberToDigitArray(PlayerGetMaxAmmo(weapon_ammo_rockets), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(310,185, digitArray, ARRAYSIZE(digitArray), TRUE);

    // cells
    NumberToDigitArray(PlayerGetAmmo(weapon_ammo_cells), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(283,191, digitArray, ARRAYSIZE(digitArray), TRUE);

    NumberToDigitArray(PlayerGetMaxAmmo(weapon_ammo_cells), digitArray, ARRAYSIZE(digitArray));
    DrawDigits(310,191, digitArray, ARRAYSIZE(digitArray), TRUE);
}

void HudRenderWeaponNumbers()
{
    // pistol (weapon 2)
    if (PlayerHasWeapon(weapon_type_pistol))
    {
        GraphicsDrawSprite(111,172, HudGetSprite("STYSNUM2"));
    }
    else
    {
        GraphicsDrawSprite(111,172, HudGetSprite("STGNUM2"));
    }

    // shotgun (weapon 3)
    if (PlayerHasWeapon(weapon_type_shotgun))
    {
        GraphicsDrawSprite(123,172, HudGetSprite("STYSNUM3"));
    }
    else
    {
        GraphicsDrawSprite(123,172, HudGetSprite("STGNUM3"));
    }

    // shotgun (weapon 4)
    if (PlayerHasWeapon(weapon_type_chaingun))
    {
        GraphicsDrawSprite(135,172, HudGetSprite("STYSNUM4"));
    }
    else
    {
        GraphicsDrawSprite(135,172, HudGetSprite("STGNUM4"));
    }

    // rocket launcher (weapon 5)
    if (PlayerHasWeapon(weapon_type_missile))
    {
        GraphicsDrawSprite(111,182, HudGetSprite("STYSNUM5"));
    }
    else
    {
        GraphicsDrawSprite(111,182, HudGetSprite("STGNUM5"));
    }

    // plasma (weapon 6)
    if (PlayerHasWeapon(weapon_type_plasma))
    {
        GraphicsDrawSprite(123,182, HudGetSprite("STYSNUM6"));
    }
    else
    {
        GraphicsDrawSprite(123,182, HudGetSprite("STGNUM6"));
    }

    // bfg (weapon 7)
    if (PlayerHasWeapon(weapon_type_bfg))
    {
        GraphicsDrawSprite(135,182, HudGetSprite("STYSNUM7"));
    }
    else
    {
        GraphicsDrawSprite(135,182, HudGetSprite("STGNUM7"));
    }
}

void HudCalculateWeaponBob()
{
    float pos;
    g_gunBobAnimationTime += GameTimeGetElapsed();

    pos = (float)sin(g_gunBobAnimationTime * 3.4);
    pos = (float)fabs(pos);
    g_gunBobY = (int)(pos * 20);

    pos = (float)cos(g_gunBobAnimationTime * 3.4);
    g_gunBobX = (int)(pos * 20);
}

int HudGetWeaponBobY()
{
    if (!InputIsKeyHeld(VK_UP) && !InputIsKeyHeld(VK_DOWN) || GameGetPause())
    {
        g_gunBobY = 0;
    }
    
    return g_gunBobY;
}

int HudGetWeaponBobX()
{
    if (!InputIsKeyHeld(VK_UP) && !InputIsKeyHeld(VK_DOWN) || GameGetPause())
    {
        g_gunBobX = 0;
    }

    return g_gunBobX;
}

BOOL WeaponIsFireFrame(weapon_type_t weaponType)
{
    return (g_weaponFrames[weaponType].CurrentFrame == (g_weaponFrames[weaponType].FireFrame));
}

BOOL WeaponIsLastFrame(weapon_type_t weaponType)
{
    return (g_weaponFrames[weaponType].CurrentFrame == (g_weaponFrames[weaponType].FrameCount - 1));
}

void UpdateWeaponNextFrame(weapon_type_t weaponType)
{
    g_gunFireAnimationTime += GameTimeGetElapsed();
    if (g_gunFireAnimationTime > g_weaponFrames[weaponType].TimeBetweenFrame)
    {
        g_weaponFrames[weaponType].CurrentFrame++;
        if (g_weaponFrames[weaponType].CurrentFrame >= g_weaponFrames[weaponType].FrameCount)
        {
            g_weaponFrames[weaponType].CurrentFrame = WEAPON_IDLE_FRAME;
        }

        // On fire animation frame, spend ammo
        if(WeaponIsFireFrame(weaponType))
        {
            PlayerUseAmmo();
        }

        g_gunFireAnimationTime = 0;
    }
}

char* WeaponGetNextFrame(weapon_type_t weaponType)
{
    return g_weaponFrames[weaponType].Frames[g_weaponFrames[weaponType].CurrentFrame].Frame;
}

char* WeaponGetNextOverlayFrame(weapon_type_t weaponType)
{
    return g_weaponFrames[weaponType].Frames[g_weaponFrames[weaponType].CurrentFrame].Overlay;
}

char* WeaponGetIdleFrame(weapon_type_t weaponType)
{
    return g_weaponFrames[weaponType].Frames[0].Frame;
}

void HudRenderWeapon()
{
    int weaponY = 18 + HudGetWeaponBobY();
    int weaponX = 0 + HudGetWeaponBobX();
    weapon_type_t weaponType = PlayerGetWeapon();

    if (PlayerIsShooting())
    {
        UpdateWeaponNextFrame(weaponType);
        
        GraphicsDrawSprite(weaponX,weaponY, HudGetSprite(WeaponGetNextFrame(weaponType)));
        if (WeaponGetNextOverlayFrame(weaponType))
        {
            GraphicsDrawSprite(weaponX,weaponY, HudGetSprite(WeaponGetNextOverlayFrame(weaponType)));
        }

        if (WeaponIsLastFrame(weaponType))
        {
            g_weaponFrames[weaponType].CurrentFrame = 0; // reset
            PlayerSetShooting(FALSE);
            return;
        }
    }
    else
    {
        g_weaponFrames[weaponType].CurrentFrame = 0; // reset
        GraphicsDrawSprite(weaponX,weaponY, HudGetSprite(WeaponGetIdleFrame(weaponType)));
    }
}

void HudRenderKeys()
{
    if (PlayerHasKey(key_type_blue))
    {
        GraphicsDrawSprite(239,171, HudGetSprite("STKEYS0"));
    }

    if (PlayerHasKey(key_type_yellow))
    {
        GraphicsDrawSprite(239,181, HudGetSprite("STKEYS1"));
    }

    if (PlayerHasKey(key_type_red))
    {
        GraphicsDrawSprite(239,191, HudGetSprite("STKEYS2"));
    }

    if (PlayerHasKey(key_type_blue_skull))
    {
        GraphicsDrawSprite(239,171, HudGetSprite("STKEYS3"));
    }

    if (PlayerHasKey(key_type_yellow_skull))
    {
        GraphicsDrawSprite(239,181, HudGetSprite("STKEYS4"));
    }

    if (PlayerHasKey(key_type_red_skull))
    {
        GraphicsDrawSprite(239,191, HudGetSprite("STKEYS5"));
    }
}

void HudClearMessages()
{
    ZeroMemory(g_hudMessageText, sizeof(g_hudMessageText));
}

void HudRenderText(int x, int y, char* text)
{
    int startX = x;
    size_t i = 0;
    char fontcharName[16];
    ZeroMemory(fontcharName, sizeof(fontcharName));

    for (i = 0; i < strlen(text); i++)
    {
        if (text[i] == VK_SPACE)
        {
            // render nothing...
            x += FONTCHARACTER_WIDTH;
        }
        else if (text[i] == '\n')
        {
            y += FONTCHARACTER_WIDTH; // (width is also height of font)
            x = startX;
        }
        else
        {
            sprintf_s(fontcharName, sizeof(fontcharName),"STCFN0%d",toupper(text[i]));
            GraphicsDrawSprite(x,y, HudGetSprite(fontcharName));
            x += ((text[i] != 'i' && text[i] != '!') ? FONTCHARACTER_WIDTH : FONTCHARACTER_WIDTH/2);
        }
    }
}

void HudRenderMessages()
{
    HudRenderText(0,0, g_hudMessageText);

    // Handle message clear timer
    g_messageClearTime += GameTimeGetElapsed();
    if (g_messageClearTime > MESSAGE_CLEAR_INTERVAL)
    {
        HudClearMessages();
        g_messageClearTime = 0;
    }
}

void HudPrintMessage(char* messageText)
{
    HudClearMessages();
    g_messageClearTime = 0;

    if (messageText)
    {
        strncpy_s(g_hudMessageText, ARRAYSIZE(g_hudMessageText), messageText, ARRAYSIZE(g_hudMessageText) - 1);
    }
}

void HudRender()
{
    HudCalculateWeaponBob();

    HudRenderWeapon();

    GraphicsDrawSprite(0,168, HudGetSprite("STBAR"));

    // If we are not in multiplayer mode, render ARMS section in the hud, otherwise
    // it will say FRAGS, which is a deathmatch term for kills.
    GraphicsDrawSprite(104,168, HudGetSprite("STARMS"));

    HudRenderAnimatedFace(PlayerGetHealth());
    HudRenderHealth(PlayerGetHealth());
    
    // If the fist or chainsaw is selected, avoid showing 0 for the ammo count.  The original
    // game just leaves the cell blank on the hud
    if (PlayerGetWeapon() != weapon_type_fist && PlayerGetWeapon() != weapon_type_chainsaw)
    {
        HudRenderAmmo(PlayerGetAmmoByWeapon(PlayerGetWeapon()));
    }
    HudRenderArmor(PlayerGetArmor());
    HudRenderAmmoClipData();
    HudRenderWeaponNumbers();
    
    HudRenderKeys();

    HudRenderMessages();
}

resource_t HudGetSprite(const char* name)
{
    return ResourceManagerGet(g_HudResourceManager, resource_type_sprite, name);
}

VOID DrawDigits(int x, int y, byte_t* digits, int count, BOOL smallDigits)
{
    BYTE i = 0;
    RECT dest;
    char digitName[16];
    dest.left = x;
    dest.top  = y;

    for (i = 0; i < count; i++)
    {
        if (digits[i] == DIGIT_TERMINATOR)
        {
            break;
        }

        ZeroMemory(digitName, sizeof(digitName));
        if (!smallDigits)
        {
            sprintf_s(digitName, sizeof(digitName),"STTNUM%d",digits[i]);
            GraphicsDrawSprite(dest.left,dest.top, HudGetSprite(digitName));

            dest.left -= 14;
        }
        else
        {
            sprintf_s(digitName, sizeof(digitName),"STYSNUM%d",digits[i]);
            GraphicsDrawSprite(dest.left,dest.top, HudGetSprite(digitName));

            dest.left -= 4;
        }
    }
}

VOID NumberToDigitArray(int value, byte_t* digits, int count)
{
    int temp = value;
    int i = 0;
    int totalDigits = 0;

    memset(digits, DIGIT_TERMINATOR, count);

    if (value == 0)
    {
        digits[0] = 0;
        return;
    }

    for(;temp!=0;temp=temp/10) 
    {
        totalDigits++;
    }

    if (totalDigits > count)
    {
        return;
    }

    temp = value;
    while (temp!=0)
    {
        digits[i] = temp % 10;
        temp /= 10;
        i++;
    }
}
