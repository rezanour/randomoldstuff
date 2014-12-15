#pragma once

#include <RuntimeObject.h>
#include <memory>
#include "GDKAL.h"

namespace GDK
{
    // Whereas our AudioDevice is the public interface, this object wraps up native OpenAL device/context so that
    // they stay valid as long as any OpenAL objects remain since it is required to set the OpenAL context used
    // to create an object in order to delete the object.
    class AudioContextOpenAL : public RuntimeObject<AudioContextOpenAL>
    {
    public:
        static std::shared_ptr<AudioContextOpenAL> Create();

        ~AudioContextOpenAL();

        void MakeCurrent();
        
    private:
        AudioContextOpenAL();
                
        ALCdevice *_device;
        ALCcontext *_context;
    };
}