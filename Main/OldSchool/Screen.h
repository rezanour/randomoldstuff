#pragma once

struct UpdateContext;
struct DrawContext;

struct __declspec(novtable) IScreen
{
    virtual bool SupportsFocus() const = 0;

    virtual void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) = 0;

    virtual void Draw(_In_ const DrawContext& context) = 0;
};
