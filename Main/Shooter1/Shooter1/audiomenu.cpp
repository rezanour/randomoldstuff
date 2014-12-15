#include "precomp.h"
#include "audiomenu.h"

#define AUDIOMENU_MUSICONOFF  0
#define AUDIOMENU_MUSICVOLUME 1
#define AUDIOMENU_SOUNDONOFF  2
#define AUDIOMENU_SOUNDVOLUME 3

AudioMenu::AudioMenu() :
    Menu(MenuId::AudioMenu, "Audio")
{
    _items.push_back(MenuItem(AUDIOMENU_MUSICONOFF,  ""));
    _items.push_back(MenuItem(AUDIOMENU_MUSICVOLUME, ""));
    _items.push_back(MenuItem(AUDIOMENU_SOUNDONOFF,  ""));
    _items.push_back(MenuItem(AUDIOMENU_SOUNDVOLUME, ""));

    UpdateMenuText();

    _colorTable.push_back(XMFLOAT4(Colors::Yellow));
    _colorTable.push_back(XMFLOAT4(Colors::Red));
}

void AudioMenu::OnSelected(_In_ MenuItem item)
{
    Config config = ConfigDuplicate();

    switch(item.id)
    {
        case AUDIOMENU_MUSICONOFF:
        config.MusicOn = !config.MusicOn;
        break;
        case AUDIOMENU_SOUNDONOFF:
        config.SoundOn = !config.SoundOn;
        break;
    }

    ConfigApply(config);
    UpdateMenuText();
}

void AudioMenu::OnDismiss()
{
    GetGame().ShowMenu(MenuId::OptionsMenu);
}

_Use_decl_annotations_
void AudioMenu::OnRightSelected(MenuItem item)
{
    Config config = ConfigDuplicate();

    switch(item.id)
    {
        case AUDIOMENU_MUSICONOFF:
            config.MusicOn = !config.MusicOn;
        break;
        case AUDIOMENU_MUSICVOLUME:
            config.MusicVolume++;
            if (config.MusicVolume > MaxAudioVolume)
            {
                config.MusicVolume = MaxAudioVolume;
            }
        break;
        case AUDIOMENU_SOUNDONOFF:
            config.SoundOn = !config.SoundOn;
        break;
        case AUDIOMENU_SOUNDVOLUME:
            config.SoundVolume++;
            if (config.SoundVolume > MaxAudioVolume)
            {
                config.SoundVolume = MaxAudioVolume;
            }
        break;
    }

    ConfigApply(config);
    UpdateMenuText();
}

_Use_decl_annotations_
void AudioMenu::OnLeftSelected(MenuItem item)
{
    Config config = ConfigDuplicate();

    switch(item.id)
    {
        case AUDIOMENU_MUSICONOFF:
            config.MusicOn = !config.MusicOn;
        break;
        case AUDIOMENU_MUSICVOLUME:
            config.MusicVolume--;
            if (config.MusicVolume < 0)
            {
                config.MusicVolume = 0;
            }
        break;
        case AUDIOMENU_SOUNDONOFF:
            config.SoundOn = !config.SoundOn;
        break;
        case AUDIOMENU_SOUNDVOLUME:
            config.SoundVolume--;
            if (config.SoundVolume < 0)
            {
                config.SoundVolume = 0;
            }
        break;
    }

    ConfigApply(config);
    UpdateMenuText();
}

void AudioMenu::UpdateMenuText()
{
    Config config = GetConfig();

    _items[AUDIOMENU_MUSICONOFF].text =  config.MusicOn ? "Music: \t1 On" : "Music: \t2 Off";
    _items[AUDIOMENU_SOUNDONOFF].text =  config.SoundOn ? "Sound: \t1 On" : "Sound: \t2 Off";
    _items[AUDIOMENU_MUSICVOLUME].text = "Music Volume: \t1" + std::to_string(config.MusicVolume);
    _items[AUDIOMENU_MUSICVOLUME].enabled = config.MusicOn;
    _items[AUDIOMENU_SOUNDVOLUME].text = "Sound Volume: \t1" + std::to_string(config.SoundVolume);
    _items[AUDIOMENU_SOUNDVOLUME].enabled = config.SoundOn;
}
