#pragma once

struct Config
{
    std::wstring SourceRoot;
    std::wstring DestRoot;
};

const Config* GetConfig();

void ConfigStartup();
void ConfigShutdown();
