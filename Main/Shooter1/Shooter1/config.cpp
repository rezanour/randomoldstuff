#include "precomp.h"

static Config g_config;

void ConfigLoad()
{
    // TODO: Load config from file eventually (so we can save changes to resolution, settings, etc...)
    g_config.Name = "Shooter1";
    g_config.ScreenWidth = 1280;
    g_config.ScreenHeight = 720;
    g_config.CanvasWidth = 800;
    g_config.CanvasHeight = 600;
    g_config.SoundOn = true;
    g_config.SoundVolume = 5;
    g_config.MusicOn = true;
    g_config.MusicVolume = 5;

    static const char root[] = "..\\..\\Content\\";
    g_config.ContentRoot = new char[_countof(root) + 1];
    strcpy_s(g_config.ContentRoot, _countof(root) + 1, root);

    DebugOut("Configuration loaded.\n");
}

void ConfigDestroy()
{
    if (g_config.ContentRoot)
    {
        delete [] g_config.ContentRoot;
        g_config.ContentRoot = nullptr;
    }
}

Config ConfigDuplicate()
{
    return g_config;
}

_Use_decl_annotations_
void ConfigApply(const Config& config)
{
    // TODO: update components
    g_config = config;

    if (g_config.SoundOn)
    {
        GetGame().GetAudioEngine().GetMasterVoice()->SetVolume(g_config.SoundVolume/(float)MaxAudioVolume);
    }
    else
    {
        GetGame().GetAudioEngine().GetMasterVoice()->SetVolume(0.0f);
    }
}

const Config& GetConfig()
{
    return g_config;
}
