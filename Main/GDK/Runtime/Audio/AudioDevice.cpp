#include <AudioDevice.h>
#include <GDKError.h>
#include <AudioContent.h>
#include <AudioClip.h>

#include "OpenAL/AudioDeviceOpenAL.h"
#include "XAudio/XA2AudioDevice.h"

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<AudioDevice> AudioDevice::Create(AudioDevice::Type type)
{
    switch (type)
    {
        case AudioDevice::Type::OpenAL:
            return std::shared_ptr<AudioDevice>(GDKNEW AudioDeviceOpenAL());
        case AudioDevice::Type::XAudio:
            return std::shared_ptr<AudioDevice>(GDKNEW AudioDeviceXAudio());
        default:
            RAISE_EXCEPTION("Invalid audio device type.", static_cast<int32_t>(type));
    }
}
