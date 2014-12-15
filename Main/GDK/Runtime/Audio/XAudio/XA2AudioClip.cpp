#include "XA2AudioClip.h"
#include "XA2AudioContext.h"
#include <AudioContent.h>
#include <GDKError.h>
#include <AudioDevice.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<AudioClipXAudio> AudioClipXAudio::Create(const std::shared_ptr<AudioContextXAudio>& context, const std::shared_ptr<AudioContent>& content)
{
    return std::shared_ptr<AudioClipXAudio>(GDKNEW AudioClipXAudio(context, content));
}

_Use_decl_annotations_
AudioClipXAudio::AudioClipXAudio(const std::shared_ptr<AudioContextXAudio>& context, const std::shared_ptr<AudioContent>& content) : _context(context)
{
    int16_t bitRate = content->GetBitRate();
    CHECK_TRUE(bitRate == 8 || bitRate == 16);

    int16_t channels = content->GetChannelCount();
    CHECK_RANGE(channels, 1, 2);
}

AudioClipXAudio::~AudioClipXAudio()
{
    
}