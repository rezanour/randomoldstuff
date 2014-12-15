#pragma once

uint32_t
RenderpRegisterTexture(
    _In_ PCSTR texture
    );

void
RenderpClearTextures();

void 
RenderpDrawTriangle(
    _In_ uint32_t texture,
    _In_ const GDK::Vector3& v0,
    _In_ const GDK::Vector2& tex0,
    _In_ const GDK::Vector3& v1,
    _In_ const GDK::Vector2& tex1,
    _In_ const GDK::Vector3& v2,
    _In_ const GDK::Vector2& tex2
    );
