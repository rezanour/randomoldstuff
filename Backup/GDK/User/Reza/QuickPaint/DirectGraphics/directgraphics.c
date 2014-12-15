#include "directgraphics.h"
#include "internal.h"


//
// public direct graphics methods
//

bool_t DGInitialize()
{
    bool_t result = true;

    DGDebugOut("Initializing DirectGraphics system.\n");

    result = result && DGInitializeMemoryManager();

    return result;
}

void DGUninitialize(bool_t reportMemoryLeaks)
{
    DGDebugOut("Uninitializing DirectGraphics system.\n");

    DGUninitializeMemoryManager(reportMemoryLeaks);
}

bool_t DGIsValidBpp(byte_t bpp)
{
    return (bpp == 8 || bpp == 16 || bpp == 24 || bpp == 32);
}
