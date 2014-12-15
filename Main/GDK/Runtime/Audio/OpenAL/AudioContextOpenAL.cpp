#include "AudioContextOpenAL.h"

using namespace GDK;

std::shared_ptr<AudioContextOpenAL> AudioContextOpenAL::Create()
{
    return std::shared_ptr<AudioContextOpenAL>(GDKNEW AudioContextOpenAL());
}
        
AudioContextOpenAL::AudioContextOpenAL() : _device(nullptr), _context(nullptr)
{
    CHECK_AL(_device = alcOpenDevice(nullptr));
    CHECK_AL(_context = alcCreateContext(_device, nullptr));
    CHECK_AL(alcMakeContextCurrent(_context));
}

AudioContextOpenAL::~AudioContextOpenAL()
{
    if (_context != nullptr)
    {
        CHECK_AL(alcMakeContextCurrent(nullptr));
        CHECK_AL(alcDestroyContext(_context));
        _context = nullptr;
    }
    if (_device != nullptr)
    {
        CHECK_AL(alcCloseDevice(_device));
        _device = nullptr;
    }
}

void AudioContextOpenAL::MakeCurrent()
{
    CHECK_AL(alcMakeContextCurrent(_context));
}