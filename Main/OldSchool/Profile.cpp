#include "Precomp.h"
#include "Profile.h"
#include "GraphicsSystem.h"
#include "SpriteFont.h"
#include "AssetLoading.h"
#include "Game\DebugDrawing.h"

struct ProfileEntry
{
    CodeTag Tag;
    bool Begin;
    LARGE_INTEGER Timestamp;
};

static const uint32_t MaxProfileEntriesPerFrame = 32000;
static ProfileEntry g_profileData[MaxProfileEntriesPerFrame];
static uint32_t g_numProfileEntries;

static bool g_profilerEnabled;

_Use_decl_annotations_
ProfileMark::ProfileMark(CodeTag tag)
{
    if (g_profilerEnabled && tag != CodeTag::None)
    {
        _tag = tag;

        if (g_numProfileEntries >= _countof(g_profileData))
        {
            OutputDebugString(L"Ran out of profiler space!\n");
            throw std::exception();
        }

        auto& entry = g_profileData[g_numProfileEntries++];
        entry.Tag = _tag;
        entry.Begin = true;
        QueryPerformanceCounter(&entry.Timestamp);
    }
    else
    {
        _tag = CodeTag::None;
    }
}

ProfileMark::~ProfileMark()
{
    if (g_profilerEnabled && _tag != CodeTag::None)
    {
        if (g_numProfileEntries >= _countof(g_profileData))
        {
            OutputDebugString(L"Ran out of profiler space!\n");
            throw std::exception();
        }

        auto& entry = g_profileData[g_numProfileEntries++];
        entry.Tag = _tag;
        entry.Begin = false;
        QueryPerformanceCounter(&entry.Timestamp);
    }
}

namespace Profiler
{
    _Use_decl_annotations_
    void EnableProfiler(bool enable)
    {
        g_profilerEnabled = enable;
    }

    void StartFrame()
    {
        // Reset the data for the frame.
        g_numProfileEntries = 0;
    }

    const char* GetCodeTagName(_In_ CodeTag tag)
    {
        switch (tag)
        {
        case CodeTag::None:
            return "None";

        case CodeTag::Update:
            return "Update";

        case CodeTag::Draw:
            return "Draw";

        case CodeTag::Present:
            return "Present";

        case CodeTag::Physics:
            return "Physics";

        case CodeTag::Collision:
            return "Collision";

        case CodeTag::DebugRendering:
            return "Debug Rendering";

        default:
            assert(false);
            return "INVALID";
        }
    }

    _Use_decl_annotations_
    void EndFrame(const std::shared_ptr<IGraphicsSystem>& graphics)
    {
        if (g_profilerEnabled)
        {
            static std::map<CodeTag, LARGE_INTEGER> combinedTime;
            combinedTime.clear();

            static LARGE_INTEGER frequency = { 0LL };
            if (frequency.QuadPart == 0LL)
            {
                QueryPerformanceFrequency(&frequency);
            }

            static std::shared_ptr<ISpriteFont> debugFont;
            if (debugFont == nullptr)
            {
                auto assetLoader = AssetLoader::Create();
                debugFont = assetLoader->LoadSpriteFontFromFile(graphics, L"font14.spritefont");
            }

            // Coalesce markers and tally stats
            for (uint32_t i = 0; i < static_cast<uint32_t>(CodeTag::Max); ++i)
            {
                CodeTag tag = static_cast<CodeTag>(i);

                ProfileEntry* entry = g_profileData;
                uint32_t recursionCount = 0;
                LARGE_INTEGER start = { 0LL };
                LARGE_INTEGER total = { 0LL };
                bool encountered = false;
                for (uint32_t n = 0; n < g_numProfileEntries; ++n, ++entry)
                {
                    if (entry->Tag == tag)
                    {
                        encountered = true;

                        if (entry->Begin)
                        {
                            ++recursionCount;
                        }
                        else
                        {
                            --recursionCount;
                        }

                        if (recursionCount == 1)
                        {
                            start = entry->Timestamp;
                        }
                        else if (recursionCount == 0)
                        {
                            total.QuadPart += (entry->Timestamp.QuadPart - start.QuadPart);
                        }
                    }
                }

                if (encountered)
                {
                    combinedTime[tag] = total;
                }
            }

            // Display stats (Just simple text now. Build better display laters)
            char message[100] = {};
            uint32_t y = 60;
            auto present = combinedTime.find(CodeTag::Present);
            for (auto& pair : combinedTime)
            {
                float time = pair.second.QuadPart * 1000.0f / static_cast<float>(frequency.QuadPart);

                if (pair.first == CodeTag::Draw && present != std::end(combinedTime))
                {
                    sprintf_s(message, "%s %3.2fms   Excl. Present: %3.2fms\n",
                        GetCodeTagName(pair.first),
                        time, time - (present->second.QuadPart * 1000.0f / static_cast<float>(frequency.QuadPart)));
                }
                else
                {
                    sprintf_s(message, "%s %3.2fms\n", GetCodeTagName(pair.first), pair.second.QuadPart * 1000.0f / static_cast<float>(frequency.QuadPart));
                }

                DrawDebugText(graphics, debugFont, 0, y, message);
                y += 20;
            }
        }
    }
}
