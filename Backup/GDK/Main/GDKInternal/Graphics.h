#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_GRAPHICS_H_
#define _GDK_GRAPHICS_H_

namespace GDK
{
    // Any graphics system usable by the engine must implement
    // the interface below and make themselves accessible to the 
    // host.

    // forward declarations
    struct IMemoryStats;

    struct IGameObject;
    struct IComponent;
    struct ICamera;

    struct IRendererScene : IRefCounted
    {
        GDK_IMETHOD_(bool) IsVisible() const = 0;
        GDK_IMETHOD_(void) SetVisible(_In_ bool visible) = 0;

        GDK_IMETHOD_(void) AddComponent(_In_ const stde::ref_ptr<IComponent>& component) = 0;
        GDK_IMETHOD_(void) RemoveComponent(_In_ const stde::ref_ptr<IComponent>& component) = 0;

        GDK_IMETHOD_(void) SetActiveCamera(_In_ const stde::ref_ptr<ICamera>& camera) = 0;
    };

    struct IRenderer : IRefCounted
    {
        GDK_IMETHOD_(void) AddScene(_In_ const stde::ref_ptr<IRendererScene>& scene) = 0;
        GDK_IMETHOD_(void) GetScenes(_Inout_ std::vector<stde::ref_ptr<IRendererScene>>& scenes) const = 0;

        // Causes all visible scenes to be rendered
        GDK_IMETHOD_(void) RenderAll() = 0;

        // access to memory stats
        GDK_IMETHOD_(stde::ref_ptr<IMemoryStats>) GetMemoryStats() = 0;
    };

} // GDK

#endif // _GDK_GRAPHICS_H_
