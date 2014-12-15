#pragma once

#include <GDK\RefCounted.h>
#include <GDK\ObjectModel\ObjectComponent.h>

#include "GraphicsTypes.h"
#include <stde\non_copyable.h>

#include "RenderTask.h"
#include "Geometry.h"

namespace Lucid3D
{
    class Renderer;

    class LightComponent;
    typedef stde::ref_counted_ptr<LightComponent> LightPtr;

    class LightComponent : stde::non_copyable, public GDK::RefCounted<GDK::ILightComponent>
    {
    public:
        static HRESULT Create(  _In_ Renderer* pRenderer, 
                                _In_ GDK::IGameObject* pGameObject, 
                                _In_ uint64 contentId,
                                _Out_ LightPtr& spComponent);

        // IObjectComponent
        GDK_IMETHOD_(const GDK::IGameObject*) GetOwner() const;
        GDK_IMETHOD_(GDK::ObjectComponentType) GetType() const;

        // Methods
        GDK_IMETHOD Process(_Inout_ RenderMap& renderMap);
        GDK_IMETHOD_(const Lucid3D::Vector3&) GetColor() const { return _color; }

    private:
        LightComponent(_In_ Renderer* pRenderer, _In_ GDK::IGameObject* pGameObject, _In_ GeometryPtr& spGeometry, _In_ const Lucid3D::Vector3& color);

        Renderer* _pRenderer;
        GDK::IGameObject* _pOwner;
        GeometryPtr _spQuad;
        Lucid3D::Vector3 _color;
    };
}
