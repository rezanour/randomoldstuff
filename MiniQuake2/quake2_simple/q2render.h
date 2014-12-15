#pragma once

void
RenderInit(
    HWND hwnd,
    int screenWidth,
    int screenHeight
    );

void RenderShutdown();

void Render(
    const GDK::Vector3& playerPosition,
    float playerAngle
    );
