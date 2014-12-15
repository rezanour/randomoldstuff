#include "Precomp.h"
#include "DebugDrawing.h"
#include "StlExtensions.h"
#include "GraphicsSystem.h"
#include "SpriteFont.h"

_Use_decl_annotations_
void DrawDebugText(const std::shared_ptr<IGraphicsSystem>& graphics, const std::shared_ptr<ISpriteFont>& spriteFont, uint32_t x, uint32_t y, const char* text, ...)
{
    char message[2048] = { 0 };
    va_list list;
    va_start(list, text);
    if (vsprintf_s(message, (sizeof(message)-2), text, list) < 0)
    {
        throw std::exception();
    }

    spriteFont->Draw(graphics, std::AnsiToWide(message).c_str(), x, y);

    va_end(list);
}
