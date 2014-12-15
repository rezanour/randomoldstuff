#include "StdAfx.h"

#ifdef BUILD_TESTS

#include "Testing.h"
#include "GameApplication.h"

using namespace CoreServices;

namespace Lucid3D
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
        CHECKHR(Configuration::Create(&spConfiguration));

        {
            GameApplication game(spConfiguration);

            CHECKHR(game.Initialize());

            game.Shutdown();
            game.Exit();
        }

    EXIT
        return SUCCEEDED(hr);
    }
}

using namespace Lucid3D;

bool GameApplicationTests()
{
    return RunSubTests("GameApplicationTests", subTests, _countof(subTests));
}

#endif

