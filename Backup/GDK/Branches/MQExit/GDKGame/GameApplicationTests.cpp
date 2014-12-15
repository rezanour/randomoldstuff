#include "StdAfx.h"

#ifdef BUILD_TESTS

#include "Testing.h"
#include "GameApplication.h"

using namespace CoreServices;

namespace GDK
{
    bool RunAndExitTest();

    static SubTest subTests[] =
    {
        // Run the test twice as it found a couple issues that way
        DefineSubTest(RunAndExitTest),
        DefineSubTest(RunAndExitTest),
    };

    bool RunAndExitTest()
    {
        HRESULT hr = S_OK;

        ConfigurationPtr spConfiguration;
        FATAL(Configuration::Create(&spConfiguration));

        {
            GameApplication game(spConfiguration);

            FATAL(game.Initialize());

            game.Shutdown();
            game.Exit();
        }

    EXIT
        return SUCCEEDED(hr);
    }
}

using namespace GDK;

bool GameApplicationTests()
{
    return RunSubTests("GameApplicationTests", subTests, _countof(subTests));
}

#endif

