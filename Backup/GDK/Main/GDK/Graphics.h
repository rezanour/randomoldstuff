// All of the Graphics interfaces are expected to be implemented by an implementation DLL, which will be loaded at runtime.
#pragma once

#include "Platform.h"
#include "RefCounted.h"
#include "Subsystem.h"
#include "ObjectModel\GameObject.h"
#include "ObjectModel\ObjectComponent.h"

namespace GDK
{
    struct RendererViewSettings
    {
        const void* WindowIdentity;
        bool        IsFullScreen;
        size_t      ScreenWidth;
        size_t      ScreenHeight;
    };

    struct GraphicsSettings
    {
        RendererViewSettings DefaultView;
    };

    struct IRendererView : public IRefCounted
    {
        GDK_IMETHOD_(const void*) GetWindowIdentity() const = 0;

        GDK_IMETHOD_(bool) IsFullScreen() const = 0;
        GDK_IMETHOD SetFullScreen(_In_ bool fullScreen) = 0;

        GDK_IMETHOD GetSize(_Out_ size_t* pWidth, _Out_ size_t* pHeight) const = 0;
        GDK_IMETHOD SetSize(_In_ size_t width, _In_ size_t height) = 0;
    };

    struct IRendererScene : public IRefCounted
    {
        GDK_IMETHOD_(bool) IsVisible() const = 0;
        GDK_IMETHOD_(void) SetVisible(_In_ bool visible) = 0;

        GDK_IMETHOD CreateComponent(_In_ IGameObject* pGameObject, _In_ uint64 contentId, _Deref_out_ IObjectComponent** ppComponent) = 0;
        GDK_IMETHOD RemoveComponent(_In_ IObjectComponent* pComponent) = 0;

        GDK_IMETHOD SetActiveCamera(_In_ IObjectComponent* pComponent) = 0;

        // Associate a view with the scene. When Render() is called, the scene is 
        // rendered to all associated views.
        GDK_IMETHOD AssociateView(_In_ IRendererView* view) = 0;

        // Disassociate a view. The scene will no longer be rendered into that view
        GDK_IMETHOD DisassociateView(_In_ IRendererView* view) = 0;

        // Get a list of all views currently associated with the scene
        GDK_IMETHOD GetAssociatedViews(_Out_cap_(maxViews) IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews) = 0;
    };

    struct IRenderer : public ISubsystem
    {
        GDK_IMETHOD Initialize(_In_ const GraphicsSettings& settings) = 0;

        GDK_IMETHOD CreateView(_In_ const RendererViewSettings& viewSettings, _Deref_out_ IRendererView** ppView) = 0;
        GDK_IMETHOD CreateScene(_Deref_out_ IRendererScene** ppScene) = 0;

        // Get the renderer's views and scenes
        GDK_IMETHOD GetViews(_Out_cap_(maxViews) IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews) = 0;
        GDK_IMETHOD GetScenes(_Out_cap_(maxScenes) IRendererScene** scenes, _In_ size_t maxScenes, _Out_ size_t* pNumScenes) = 0;

        // Causes all visible scenes to be rendered to their associated views
        GDK_IMETHOD Render() = 0;
    };
}

