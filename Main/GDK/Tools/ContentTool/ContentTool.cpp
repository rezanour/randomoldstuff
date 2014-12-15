#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "macros.h"
#include "helpers.h"
#include "bmp.h"
#include "pcx.h"
#include "file.h"
#include "pak.h"
#include "wal.h"
#include "md2.h"
#include "bsp.h"
#include "wl6.h"

void ExtractAllContent(_In_ const std::wstring& path, _In_ const std::wstring& outputDirectory);
void ExtractAllWolf3DContent(_In_ const std::wstring& path, _In_ const std::wstring& outputDirectory);

bool g_convertToBMP = false;
bool g_listPAKContents = false;

bool g_extractPCX = false;
bool g_extractWAL = false;
bool g_extractMD2 = false;
bool g_extractBSP = false;
bool g_extractWAV = false;

bool g_extractWL6 = false;
bool g_extractWL6_WALL = false;
bool g_extractWL6_SPRITE = false;
bool g_extractWL6_MAP = false;
bool g_generateWL6_Models = false;

float g_scaleFactor = 1.0f;

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 4)
    {
        printf("Extracts and converts content from a Quake II .PAK file into GDK supported formats.\n\n");
        printf("CONTENTTOOL [pakfile] [directory] [/a] [/w] [/m] [/b] [/v] [/p] [/i] [/l]\n\n");
        printf("  /a  Process all content\n");
        printf("  /w  Process WAL content\n");
        printf("  /m  Process MD2 content\n");
        printf("  /b  Process BSP content\n");
        printf("  /v  Process WAV content\n");
        printf("  /p  Process PCX content\n");
        printf("  /i  Process WAL and PCX into BMP\n");
        printf("  /l  List contents of PAK file.  Additional command line options will be ignored.\n");
        printf("\n");
        printf("CONTENTTOOL [wl6 files directory] [directory] wolf3d [/w] [/s] [/m] [/c]\n\n");
        printf("  /w Process WALL content\n");
        printf("  /s Process SPRITE content\n");
        printf("  /m Process MAP contents\n");
        printf("  /g Generate Models\n");
        printf("  /c[n] Scale Processed MAP content (n X object's current scale)\n");
        printf("        This must be the last switch.  Example: c10 scales output 10x\n");
        return -1;
    }

    // Commandline switches can be combined together
    // Example: "vmw" = Process WAV, MD2 and WAL only.
    for (int argIndex = 3; argIndex < argc; argIndex++)
    {
        std::wstring arg = argv[argIndex];
        if (arg.rfind(L"wolf3d") != std::wstring::npos)
        {
            g_scaleFactor = 80.0f; // Default to a compatible scale factor for use with Quake 2 content
            g_extractWL6 = true;
            if (argIndex + 1 < argc)
            {
                argIndex++;
                arg = argv[argIndex];
            }
        }

        for (size_t i = 0; i < arg.size(); i++)
        {
            switch(arg[i])
            {
            case 'a':
                g_extractPCX = true;
                g_extractWAL = true;
                g_extractMD2 = true;
                g_extractBSP = true;
                g_extractWAV = true;
                break;
            case 'w':
                g_extractWAL = true;
                g_extractWL6_WALL = true;
                break;
            case 'm':
                g_extractMD2 = true;
                g_extractWL6_MAP = true;
                break;
            case 'b':
                g_extractBSP = true;
                break;
            case 'v':
                g_extractWAV = true;
                break;
            case 'p':
                g_extractPCX = true;
                break;
            case 'i':
                g_convertToBMP = true;
                break;
            case 'l':
                g_listPAKContents = true;
                break;
            case 's':
                g_extractWL6_SPRITE = true;
                break;
            case 'g':
                g_generateWL6_Models = true;
                break;
            case 'c':
                // Must be the last argument according to this tool's design.
                {
                    std::wstring scaleFactor;
                    i++; // skip argument and start parsing scale value
                    for (; i < arg.size(); i++)
                    {
                        scaleFactor += arg[i];
                    }
                    g_scaleFactor = (float)_wtof(scaleFactor.c_str());
                }
                break;
            default:
                break;
            }
        }
    }

    if (g_extractWL6)
    {
        // Extract all .wl6 Wolfenstein 3D content
        ExtractAllWolf3DContent(argv[1], argv[2]);
    }
    else
    {
        // Extract all content
        ExtractAllContent(argv[1], argv[2]);
    }

    return 0;
}

void ExtractAllContent(_In_ const std::wstring& path, _In_ const std::wstring& outputDirectory)
{
    std::ifstream inputfile(path,std::ios::in|std::ios_base::binary);
    if (inputfile.is_open())
    {
        std::vector<bmp_paletteentry_t> palette;
        std::vector<pak_entry_t> entries;
        ReadPakEntryList(path, entries);

        // Get the common palette for converting .wal data
        GetCommonPalette(inputfile, entries, palette);

        for(size_t i = 0; i < entries.size(); i++)
        {
            if (g_listPAKContents)
            {
                printf("%s = %d bytes\n", GetPakEntryName(entries[i]).c_str() ,entries[i].length);
            }
            else
            {
                std::wstring outputName;
                std::vector<uint8_t> inputbuffer;
                std::vector<uint8_t> outputBuffer;

                // Create directory for content
                std::wstring pakEntryRootDirectory;
                std::wstring pakEntryFullPath = outputDirectory;
                std::wstring entryName = AnsiToWide(GetPakEntryName(entries[i]));
                NormalizePathSlashes(entryName);
                pakEntryFullPath.append(L"\\");
                pakEntryFullPath.append(entryName);
                pakEntryRootDirectory = DirectoryRootFromPath(pakEntryFullPath);
                CreateFullDirectory(pakEntryRootDirectory);

                // Read PAK content into buffer and pass to correct conversion method
                ReadPakEntryToBuffer(inputfile, entries[i], inputbuffer);

                outputName = pakEntryFullPath;

                // Convert .pcx to .texture
                if ((entryName.rfind(L".pcx") != std::wstring::npos) && g_extractPCX)
                {
                    ConvertPCXTo32bitBMP(inputbuffer, outputBuffer);

                    if (g_convertToBMP)
                    {
                        std::wstring bmpName = outputName;
                        bmpName.append(L".bmp");
                        printf("saving '%ws' to '%ws'\n", entryName.c_str(), bmpName.c_str());
                        WriteBufferToFile(bmpName, outputBuffer);
                    }

                    std::shared_ptr<GDK::TextureContent> content;
                    outputName.append(L".texture");
                    printf("converting '%ws' to '%ws'...\n", entryName.c_str(), outputName.c_str());
                    Convert32bitBMPToTextureContent(outputBuffer, content);
                    WriteTextureToFile(outputName, content);
                }
            
                // Convert .md2 to .geometry
                else if ((entryName.rfind(L".md2") != std::wstring::npos) && g_extractMD2)
                {
                    std::shared_ptr<GDK::GeometryContent> content;
                    outputName.append(L".geometry");
                    printf("converting '%ws' to '%ws'\n", entryName.c_str(), outputName.c_str());
                    ConvertMD2ToGeometryContent(inputbuffer, content);
                    WriteGeometryToFile(outputName, content);
                }

                // Convert .wal to .texture
                else if ((entryName.rfind(L".wal") != std::wstring::npos) && g_extractWAL)
                {
                    ConvertWALTo32bitBMP(inputbuffer, palette, outputBuffer);

                    if (g_convertToBMP)
                    {
                        std::wstring bmpName = outputName;
                        bmpName.append(L".bmp");
                        printf("converting '%ws' to '%ws'\n", entryName.c_str(), bmpName.c_str());
                        WriteBufferToFile(bmpName, outputBuffer);
                    }

                    std::shared_ptr<GDK::TextureContent> content;
                    outputName.append(L".texture");
                    printf("converting '%ws' to '%ws'\n", entryName.c_str(), outputName.c_str());
                    Convert32bitBMPToTextureContent(outputBuffer, content);
                    WriteTextureToFile(outputName, content);
                }

                // Convert .bsp to .world
                else if ((entryName.rfind(L".bsp") != std::wstring::npos) && g_extractBSP)
                {
                    std::vector<std::shared_ptr<GDK::GeometryContent>> contentList;
                    std::shared_ptr<GDK::GameWorldContent> world;
                    std::vector<std::wstring> textureNameList;
                    std::vector<std::wstring> modelNameList;

                    printf("converting '%ws' to '%ws'\n", entryName.c_str(), outputName.c_str());

                    // Create a .world directory
                    CreateFullDirectory(outputName);

#if 0
                    // Extract bsp entities file
                    std::vector<uint8_t> entitiesBuffer;
                    ConvertBSPIntoEntitiesBuffer(inputbuffer, entitiesBuffer);
                    std::wstring entitiesFileName = outputName;
                    entitiesFileName.append(L"\\entities.txt");
                    WriteBufferToFile(entitiesFileName, entitiesBuffer);
#endif
                    // Convert the .bsp into a .world file and associated list of geometry and textures
                    ConvertBSPIntoWorldContentAndContentList(entryName, inputfile, entries, inputbuffer, textureNameList, modelNameList, contentList, world);

                    WriteWorldToFile(outputName + L"\\data.world", world);

                    for (size_t i = 0; i < contentList.size(); i++)
                    {
                        // Save the geometry with a name closely matching the texture's name.
                        // Example: A geometry associated with the texture "eight/equals/equals/d.wal" will be named
                        //          "eight%equals%equals%d.wal.geometry"
                        std::wstring geometryName = outputName;
                        geometryName.append(L"\\");
                        geometryName.append(modelNameList[i]);

                        // Ensure subdirectory exists
                        CreateFullDirectory(geometryName);

                        ReplaceCharacter(textureNameList[i], L'/', L'%');
                        geometryName.append(textureNameList[i]);
                        geometryName.append(L".geometry");

                        WriteGeometryToFile(geometryName, contentList[i]);
                    }
                }

                // Extract .wav
                else if ((entryName.rfind(L".wav") != std::wstring::npos) && g_extractWAV)
                {
                    printf("extracting '%ws' to '%ws'\n", entryName.c_str(), outputName.c_str());
                    WriteBufferToFile(outputName, inputbuffer);
                }

                else
                {
                    printf("skipping '%ws'\n", entryName.c_str());
                }
            }
        }
    }
    else
    {
        printf("Failed to open PAK file %ws\n", path.c_str());
    }
}

void ExtractAllWolf3DContent(_In_ const std::wstring& path, _In_ const std::wstring& outputDirectory)
{
    std::vector<uint8_t> mapheadBuffer;
    std::vector<uint8_t> gamemapsBuffer;
    std::vector<uint8_t> vgaheadBuffer;
    std::vector<uint8_t> vgadictBuffer;
    std::vector<uint8_t> vswapBuffer;
    std::vector<uint8_t> vgagraphBuffer;

    std::vector<uint8_t> inputbuffer;
    std::vector<uint8_t> outputbuffer;

    std::wstring outputName;

    ReadFileToBuffer(path + L"\\maphead.wl6", mapheadBuffer);
    ReadFileToBuffer(path + L"\\gamemaps.wl6", gamemapsBuffer);
    ReadFileToBuffer(path + L"\\vgahead.wl6", vgaheadBuffer);
    ReadFileToBuffer(path + L"\\vgadict.wl6", vgadictBuffer);
    ReadFileToBuffer(path + L"\\vgagraph.wl6", vgagraphBuffer);
    ReadFileToBuffer(path + L"\\vswap.wl6", vswapBuffer);

    // Create output directory
    CreateFullDirectory(outputDirectory);

    if (g_extractWL6_WALL)
    {
        // Extract walls
        std::vector<w3d_VSwapChunkInfo_t> vswapChunks;
        ReadVSwapChunkList(vswapBuffer, wallsChunksOnly, vswapChunks);
        for (size_t i = 0; i < vswapChunks.size(); i++)
        {
            wchar_t wallId[4] = {0};
            _itow_s((int)vswapChunks[i].id, wallId, _countof(wallId), 10);

            outputName = outputDirectory + L"\\textures\\walls\\";

            CreateFullDirectory(outputName);

            outputName.append(wallId);
            outputName.append(L".texture");

            std::vector<uint8_t> wall;
            std::shared_ptr<GDK::TextureContent> content;
            ReadVSwapChunkToBuffer(vswapBuffer, vswapChunks[i].chunk, wall);
            ConvertWL6WallTo32bitBMP(wall, inputbuffer);
            Convert32bitBMPToTextureContent(inputbuffer, content);
            printf("extracting wall texture to '%ws'\n", outputName.c_str());
            WriteTextureToFile(outputName, content);
        }

        // Add fabricated floor texture
        std::shared_ptr<GDK::TextureContent> content;
        Create1x1Texture(80, 80, 80, 255, content);
        outputName = outputDirectory + L"\\textures\\walls\\";
        CreateFullDirectory(outputName);
        outputName.append(L"1000.texture");
        printf("extracting fabricated wall texture to '%ws'\n", outputName.c_str());
        WriteTextureToFile(outputName, content);
    }

    if (g_extractWL6_SPRITE)
    {
        // Extract sprites
        std::vector<w3d_VSwapChunkInfo_t> vswapChunks;
        ReadVSwapChunkList(vswapBuffer, spriteChunksOnly, vswapChunks);
        for (size_t i = 0; i < vswapChunks.size(); i++)
        {
            wchar_t spriteId[4] = {0};
            _itow_s((int)vswapChunks[i].id, spriteId, _countof(spriteId), 10);

            outputName = outputDirectory + L"\\textures\\sprites\\";

            CreateFullDirectory(outputName);

            outputName.append(spriteId);
            outputName.append(L".texture");

            std::vector<uint8_t> sprite;
            std::shared_ptr<GDK::TextureContent> content;
            ReadVSwapChunkToBuffer(vswapBuffer, vswapChunks[i].chunk, sprite);
            ConvertWL6SpriteTo32bitBMP(sprite, inputbuffer);
            Convert32bitBMPToTextureContent(inputbuffer, content);
            printf("extracting sprite to '%ws'\n", outputName.c_str());
            WriteTextureToFile(outputName, content);
        }
    }

    if (g_extractWL6_MAP)
    {
        // Extract maps
        std::vector<w3d_level_t> levels;
        ReadW3DLevelList(mapheadBuffer, gamemapsBuffer, levels);
        for (size_t i = 0; i < levels.size(); i++)
        {
            outputName = outputDirectory + L"\\maps\\";
            outputName.append(AnsiToWide(GetW3DLevelName(levels[i])));
            CreateFullDirectory(outputName);

            printf("extracting map to '%ws'\n", outputName.c_str());

            std::vector<uint8_t> level;
            std::vector<std::shared_ptr<GDK::GeometryContent>> contentList;
            std::vector<std::wstring> textureNameList;
            ReadW3dLevelLayerToBuffer(gamemapsBuffer, levels[i], wallsLayer ,level);
            ConvertWallsLayerToGeometryContentList(level, textureNameList, contentList, g_scaleFactor);

            for (size_t i = 0; i < contentList.size(); i++)
            {
                // Save the geometry with a name closely matching the texture's name.
                // Example: A geometry associated with the texture "eight/equals/equals/d" will be named
                //          "eight%eight%equals%d.geometry"
                std::wstring geometryName = outputName;
                geometryName.append(L"\\");
                ReplaceCharacter(textureNameList[i], L'\\', L'%');
                geometryName.append(textureNameList[i]);
                geometryName.append(L".geometry");

                WriteGeometryToFile(geometryName, contentList[i]);
            }
        }
    }

    if (g_generateWL6_Models)
    {
        std::shared_ptr<GDK::GeometryContent> doorContent;
        CreateDoorModel(g_scaleFactor, doorContent);

        outputName = outputDirectory + L"\\models\\Doors\\Standard";
        CreateFullDirectory(outputName);
        outputName.append(L"\\textures%walls%99.geometry");

        WriteGeometryToFile(outputName, doorContent);

        outputName = outputDirectory + L"\\models\\Doors\\Elevator";
        CreateFullDirectory(outputName);
        outputName.append(L"\\textures%walls%104.geometry");

        WriteGeometryToFile(outputName, doorContent);

        outputName = outputDirectory + L"\\models\\Doors\\SilverKey";
        CreateFullDirectory(outputName);
        outputName.append(L"\\textures%walls%105.geometry");

        WriteGeometryToFile(outputName, doorContent);

        outputName = outputDirectory + L"\\models\\Doors\\GoldKey";
        CreateFullDirectory(outputName);
        outputName.append(L"\\textures%walls%106.geometry");

        WriteGeometryToFile(outputName, doorContent);

        // 99 - standard door
        // 104 - elevator door
        // 105 - locked door (silver key)
        // 106 - locked door (gold key)
    }
}