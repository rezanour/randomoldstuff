#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

bool System::Initialize()
{
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        return false;
    }

    return !!ovr_Initialize();
}

void System::Shutdown()
{
    ovr_Shutdown();

    CoUninitialize();
}
