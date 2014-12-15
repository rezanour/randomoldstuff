#include "common.h"

VOID AnimationInit(PANIMATION animation, float duration, BYTE numAnimationFrames, BOOL loop)
{
    animation->CurrentFrame = 0;
    animation->CurrentTime = 0;
    animation->Duration = duration;
    animation->TotalFrames = numAnimationFrames;
    animation->Loop = loop;
}

BOOL AnimationIsValid(PANIMATION animation)
{
    return animation->TotalFrames > 0 && animation->Duration > 0;
}

BOOL AnimationUpdate(PANIMATION animation)
{
    float progress = 0.0f;

    if (!AnimationIsValid(animation))
    {
        return FALSE;
    }

    animation->CurrentTime += g_elapsedTime;

    progress = animation->CurrentTime / animation->Duration;
    animation->CurrentFrame = min(animation->TotalFrames - 1, progress * animation->TotalFrames);

    if (animation->CurrentTime >= animation->Duration)
    {
        if (animation->Loop)
        {
            animation->CurrentTime -= animation->Duration;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

BYTE AnimationGetFrame(PANIMATION animation)
{
    if (!AnimationIsValid(animation))
    {
        return 0;
    }

    return animation->CurrentFrame;
}

VOID AnimationSetFrame(PANIMATION animation, BYTE frame)
{
    animation->CurrentFrame = frame;
    animation->CurrentTime = (frame / (float)animation->TotalFrames) * animation->Duration;
}
