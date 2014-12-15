// SubsystemManager is in charge of loading/unloading DLLs and keeping track of loaded subsystems
#pragma once

#include "GDKTypes.h"
#include <stde\non_copyable.h>

namespace GDK
{
    class SubsystemManager : stde::non_copyable
    {
    public:
        SubsystemManager();
        ~SubsystemManager();

        HRESULT CreateRenderer(_In_ const std::string& module, _In_ const GDK::GraphicsSettings& settings);

        IRendererPtr GetRenderer() const { return _spRenderer; }

    private:
        IRendererPtr _spRenderer;
    };
}
