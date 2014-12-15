#pragma once

struct IGraphicsSystem;

enum class CodeTag
{
    None,
    Update,
    Draw,
    Present,
    Physics,
    Collision,
    DebugRendering,
    Max
};

class ProfileMark
{
public:
    ProfileMark(_In_ CodeTag tag);
    ~ProfileMark();

private:
    ProfileMark();
    ProfileMark(const ProfileMark&);
    ProfileMark& operator= (const ProfileMark&);

    CodeTag _tag;
};

namespace Profiler
{
    void EnableProfiler(_In_ bool enable);
    void StartFrame();
    void EndFrame(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);
}
