#include "precomp.h"

int wmain(int numArgs, _In_count_(numArgs) wchar_t* args[])
{
    if (numArgs > 1)
    {
        std::wstring path(args[1]);
        if (path[path.size() - 1] == L'\"')
        {
            path[path.size()-1] = 0;
        }
        CHECKGLE(SetCurrentDirectory(path[0] == L'\"' ? path.c_str() + 1 : path.c_str()));
    }

    CHECKHR(CoInitialize(nullptr));

    ConfigStartup();

    std::unique_ptr<TaskManager> taskManager(new TaskManager);

    // TEST
    struct ProcessStub
    {
        wchar_t Input[MAX_PATH];
        wchar_t Output[128];
    };

    ProcessStub textures[] = 
    {
        { L"Castle\\Texture\\arc14.jpg",                    L"Castle\\Texture\\arc14.dds"},
        { L"Castle\\Texture\\arc18.jpg",                    L"Castle\\Texture\\arc18.dds"},
        { L"Castle\\Texture\\arc019.jpg",                   L"Castle\\Texture\\arc019.dds"},
        { L"Castle\\Texture\\castle.jpg",                   L"Castle\\Texture\\castle.dds"},
        { L"Castle\\Texture\\cottage_roof.jpg",             L"Castle\\Texture\\cottage_roof.dds"},
        { L"Castle\\Texture\\ear2_tilepattern_bmp.bmp",     L"Castle\\Texture\\ear2_tilepattern_bmp.dds"},
        { L"Castle\\Texture\\efl2.jpg",                     L"Castle\\Texture\\efl2.dds"},
        { L"Castle\\Texture\\epil1.jpg",                    L"Castle\\Texture\\epil1.dds"},
        { L"Castle\\Texture\\epil1_b.jpg",                  L"Castle\\Texture\\epil1_b.dds"},
        { L"Castle\\Texture\\est9.jpg",                     L"Castle\\Texture\\est9.dds"},
        { L"Castle\\Texture\\est10.jpg",                    L"Castle\\Texture\\est10.dds"},
        { L"Castle\\Texture\\flag.jpg",                     L"Castle\\Texture\\flag.dds"},
        { L"Castle\\Texture\\th_mc_door_20.jpg",            L"Castle\\Texture\\th_mc_door_20.dds"},
        { L"Castle\\Texture\\th_portugal_edit-lt.jpg",      L"Castle\\Texture\\th_portugal_edit-lt.dds"},
        { L"Castle\\Texture\\ti_42.jpg",                    L"Castle\\Texture\\ti_42.dds"},
        { L"Castle\\Texture\\window_withstone2.jpg",        L"Castle\\Texture\\window_withstone2.dds"},
    };

    for (uint32_t i = 0; i < _countof(textures); ++i)
    {
        taskManager->AddTask(new TextureTask(textures[i].Input, textures[i].Output, true, 1024, 1024, true));
    }

    ProcessStub worlds[] = 
    {
        { L"Castle\\castle.obj",                            L"Castle\\castle.geometry"},
        { L"Castle\\castle.mtl",                            L"Castle\\castle.mtl"},
    };

    for (uint32_t i = 0; i < _countof(worlds); ++i)
    {
        taskManager->AddTask(new WorldTask(worlds[i].Input, worlds[i].Output));
    }

    taskManager->Run();

    ConfigShutdown();

    CoUninitialize();
    return 0;
}
