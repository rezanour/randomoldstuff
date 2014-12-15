#pragma once

const uint8_t MaxAudioVolume = 10;

struct Config
{
    Config()
        : ContentRoot(nullptr)
    {
    }

    //
    // Properties
    //

    const char* Name;
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
    int8_t SoundVolume;
    int8_t MusicVolume;
    bool SoundOn;
    bool MusicOn;

    char* ContentRoot;

    // 2D Virtual Canvas Dimensions
    uint32_t CanvasWidth;
    uint32_t CanvasHeight;
};

// Used by system to populate the config before game starts
void ConfigLoad();
void ConfigDestroy();

// Used by the menu system to get, modify, and apply new configuration
Config ConfigDuplicate();
void ConfigApply(_In_ const Config& config);

// Accessible anywhere to access global config data
const Config& GetConfig();
