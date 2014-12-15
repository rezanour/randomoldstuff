#include "AudioClipOpenAL.h"
#include "AudioContextOpenAL.h"
#include <AudioContent.h>
#include <GDKError.h>
#include <AudioDevice.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<AudioClipOpenAL> AudioClipOpenAL::Create(const std::shared_ptr<AudioContextOpenAL>& context, const std::shared_ptr<AudioContent>& content)
{
    return std::shared_ptr<AudioClipOpenAL>(GDKNEW AudioClipOpenAL(context, content));
}

_Use_decl_annotations_
AudioClipOpenAL::AudioClipOpenAL(const std::shared_ptr<AudioContextOpenAL>& context, const std::shared_ptr<AudioContent>& content) : _context(context), _bufferHandle(0)
{
    int16_t bitRate = content->GetBitRate();
    CHECK_TRUE(bitRate == 8 || bitRate == 16);

    int16_t channels = content->GetChannelCount();
    CHECK_RANGE(channels, 1, 2);
    
    ALenum format = 0;
    if (bitRate == 8)
    {
        if (channels == 1)
        {
            format = AL_FORMAT_MONO8;
        }
        else if (channels == 2)
        {
            format = AL_FORMAT_STEREO8;
        }
    }
    else if (bitRate == 16)
    {
        if (channels == 1)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (channels == 2)
        {
            format = AL_FORMAT_STEREO16;
        }
    }
    
    _context->MakeCurrent();
    CHECK_AL(alGenBuffers(1, &_bufferHandle));
    CHECK_AL(alBufferData(_bufferHandle, format, content->GetData(), content->GetDataLength(), content->GetSampleRate()));
}

AudioClipOpenAL::~AudioClipOpenAL()
{
    if (_bufferHandle != 0)
    {
        _context->MakeCurrent();
        CHECK_AL(alDeleteBuffers(1, &_bufferHandle));
        _bufferHandle = 0;
    }
}