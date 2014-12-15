#pragma once

struct IGraphicsSystem;
struct ISpriteFont;

void DrawDebugText(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const std::shared_ptr<ISpriteFont>& spriteFont, _In_ uint32_t x, _In_ uint32_t y, _In_z_ const char* text, ...);
