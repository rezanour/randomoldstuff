#include "precomp.h"
#include "config.h"

static Config g_config;

const Config* GetConfig()
{
    return &g_config;
}

void ConfigStartup()
{
    // TODO: read this in from command line
    g_config.SourceRoot = L"..\\..\\Assets\\";
    g_config.DestRoot = L"..\\..\\Content\\";
}

void ConfigShutdown()
{
}
