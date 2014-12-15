#pragma once

enum class DoorEntityState
{
    Closed,
    Opening,
    Opened,
    Closing
};

struct DoorEntityTrackingInfo
{
    DoorEntityState State;
    Transform ClosePositionTransform;
    float OpenCloseElapsedTime;
    float ElapsedTimeToComplete;
    BoundingBox BoundingBox;
    bool Triggered;
};
