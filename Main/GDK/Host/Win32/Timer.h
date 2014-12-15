#pragma once

#include <Windows.h>
#include <NonCopyable.h>

// Basic timer to simplify our main game loop
class Timer : GDK::NonCopyable
{
public:
    Timer() : _frequency(0), _previousTime(0), _elapsedTime(0), _totalTime(0)
    {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        _frequency = float(li.QuadPart);
        QueryPerformanceCounter(&li);
        _previousTime = li.QuadPart;
    }

    void Tick()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        _elapsedTime = float(li.QuadPart - _previousTime) / _frequency;
        _previousTime = li.QuadPart;
        _totalTime += _elapsedTime;
    }

    __forceinline float GetElapsedTime() const { return _elapsedTime; }
    __forceinline float GetTotalTime() const { return _totalTime; }

private:
    float _frequency;
    long long _previousTime;
    float _elapsedTime;
    float _totalTime;
};