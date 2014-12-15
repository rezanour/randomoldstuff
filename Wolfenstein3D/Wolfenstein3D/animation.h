#ifndef _ANIMATION_H_
#define _ANIMATION_H_

typedef struct
{
    float CurrentTime;
    float Duration;
    BYTE CurrentFrame;
    BYTE TotalFrames;
    BOOL Loop;
} ANIMATION, *PANIMATION;

/*
Animation methods
*/

VOID AnimationInit(PANIMATION animation, float duration, BYTE numAnimationFrames, BOOL loop);
BOOL AnimationIsValid(PANIMATION animation);

/* returns false if the animation is complete, otherwise true */
BOOL AnimationUpdate(PANIMATION animation);
BYTE AnimationGetFrame(PANIMATION animation);
VOID AnimationSetFrame(PANIMATION animation, BYTE frame);

#endif /* _ANIMATION_H_ */