#include "XA2AudioContext.h"

using namespace GDK;

std::shared_ptr<AudioContextXAudio> AudioContextXAudio::Create()
{
    return std::shared_ptr<AudioContextXAudio>(GDKNEW AudioContextXAudio());
}
        
AudioContextXAudio::AudioContextXAudio()
{
}

AudioContextXAudio::~AudioContextXAudio()
{

}
