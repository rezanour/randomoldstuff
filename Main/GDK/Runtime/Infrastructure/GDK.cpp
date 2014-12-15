#include <GDK.h>
#include <InputInternal.h>
#include <PhysicsInternal.h>
#include <ObjectTable.h>

namespace GDK
{
    uint8_t ObjectTableInternal::_nextId = 0;

    static bool g_initialized = false;

    void Startup()
    {
        if (!g_initialized)
        {
            Input::Init();
            Physics::Startup();

            g_initialized = true;
        }
    }

    void Shutdown()
    {
        if (g_initialized)
        {
            Physics::Shutdown();
            g_initialized = false;
        }
    }
}
