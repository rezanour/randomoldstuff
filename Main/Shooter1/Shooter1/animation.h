#pragma once

class AnimationManager
{
    struct Animation
    {
        uint32_t ID;
        float Start;
        float End;
        float Progress;
        float Duration;
        bool Loop : 1;
        bool Completed : 1;
        bool DeleteOnComplete : 1;
        std::function<void(float value)> Apply;

        Animation(uint32_t id, float start, float end, float duration, bool loop, const std::function<void(float)>& apply, bool deleteOnComplete);
        void Update(float elapsed);
    };

public:
    uint32_t CreateAnimation(float start, float end, float duration, bool loop, const std::function<void(float value)>& applyAction, bool deleteOnComplete);
    bool IsAnimationComplete(uint32_t animationId) const;
    void DeleteAnimation(uint32_t animationId);

    void Update(float elapsedTime);

private:
    friend void AnimationStartup();
    AnimationManager();
    AnimationManager(const AnimationManager&);
    AnimationManager& operator=(const AnimationManager&);

private:
    std::vector<Animation> _animations;
    uint32_t _nextID;
};

void AnimationStartup();
void AnimationShutdown();

AnimationManager& GetAnimations();
