#include "precomp.h"
#include "animation.h"

static AnimationManager* g_animationManager;

AnimationManager::Animation::Animation(uint32_t id, float start, float end, float duration, bool loop, const std::function<void(float)>& apply, bool deleteOnComplete) :
    ID(id), Start(start), End(end), Duration(duration), Loop(loop), Apply(apply), Progress(0), Completed(false), DeleteOnComplete(deleteOnComplete)
{
}

void AnimationManager::Animation::Update(float elapsed)
{
    if (Completed)
    {
        return;
    }

    Progress += elapsed / Duration;
    if (Progress > 1)
    {
        if (Loop)
        {
            Progress -= Duration * (int)(Progress / Duration);
        }
        else
        {
            Progress = 1;
            Completed = true;
        }
    }

    Apply(Start + Progress * (End - Start));
}

AnimationManager::AnimationManager() :
    _nextID(0)
{
}

uint32_t AnimationManager::CreateAnimation(float start, float end, float duration, bool loop, const std::function<void(float value)>& applyAction, bool deleteOnComplete)
{
    _animations.emplace_back(Animation(_nextID++, start, end, duration, loop, applyAction, deleteOnComplete));
    return _animations.back().ID;
}

bool AnimationManager::IsAnimationComplete(uint32_t animationId) const
{
    size_t min = 0;
    size_t max = _animations.size() - 1;

    while (min != max)
    {
        size_t i = min + (max - min) / 2;
        auto& a = _animations[i];

        if (a.ID == animationId)
        {
            return a.Completed;
        }
        else if (a.ID > animationId)
        {
            max = i;
        }
        else
        {
            min = i;
        }
    }

    // not found, just say it was complete
    return true;
}

void AnimationManager::DeleteAnimation(uint32_t animationId)
{
    size_t min = 0;
    size_t max = _animations.size() - 1;

    while (min != max)
    {
        size_t i = min + (max - min) / 2;
        auto& a = _animations[i];

        if (a.ID == animationId)
        {
            _animations.erase(std::vector<Animation>::iterator(&_animations[i], &_animations));
            break;
        }
        else if (a.ID > animationId)
        {
            max = i;
        }
        else
        {
            min = i;
        }
    }
}

void AnimationManager::Update(float elapsedTime)
{
    for (int i = (int)_animations.size() - 1; i >= 0; --i)
    {
        auto& a = _animations[i];

        a.Update(elapsedTime);

        if (a.Completed && a.DeleteOnComplete)
        {
            _animations.erase(std::vector<Animation>::iterator(&_animations[i], &_animations));
        }
    }
}

void AnimationStartup()
{
    assert(g_animationManager == nullptr);
    delete g_animationManager;
    g_animationManager = new AnimationManager();
}

void AnimationShutdown()
{
    assert(g_animationManager != nullptr);
    delete g_animationManager;
    g_animationManager = nullptr;
}

AnimationManager& GetAnimations()
{
    assert(g_animationManager != nullptr);
    return *g_animationManager;
}
