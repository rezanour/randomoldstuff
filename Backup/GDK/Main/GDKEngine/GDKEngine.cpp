#include "StdAfx.h"

extern "C" HRESULT GDK::CreateEngine(_In_ GDK::IGameHost* host, _In_ IStream* configData, _Deref_out_ GDK::IEngine** engine)
{
    try
    {
        assert(host);
        assert(configData);
        assert(engine);

        *engine = nullptr;

        stde::ref_ptr<Configuration> configuration;

        CHK(Configuration::Load(configData, &configuration));

        configuration->SetValue(L"poo", L"smells");
        configuration->SetValue(L"fart", true);
        configuration->SetValue(L"dookie", 37);
        configuration->SetValue(L"what gives?", 12.45f);

        if (configuration->GetBool(L"fart"))
        {
            if (37 == configuration->GetInt(L"dookie"))
            {
                DebugString(configuration->GetString(L"poo"));
                DebugString(L"\n");
            }
        }

        EngineCorePtr core = EngineCore::Create(host);
        *engine = core.detach();
    }
    catch (const std::exception&)
    {
        return E_FAIL;
    }

    return S_OK;
}
