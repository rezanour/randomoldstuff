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

    class LightComponent : stde::non_copyable, public GDK::RefCountedBase<GDK::ILightComponent>
    {
    public:
        static HRESULT Create(  _In_ Renderer* pRenderer, 
                                _In_ GDK::IGameObject* pGameObject, 
                                _In_ uint64 contentId,
                                _Out_ LightPtr& spComponent);

        // IObjectComponent
        GDK_METHOD_(const GDK::IGameObject*) GetOwner() const;
        GDK_METHOD_(GDK::ObjectComponentType) GetType() const;

        // Methods
        GDK_METHOD Process(_Inout_ RenderMap& renderMap);
        GDK_METHOD_(const L3DMath::Vector3&) GetColor() const { return _color; }

    private:
        LightComponent(_In_ Renderer* pRenderer, _In_ GDK::IGameObject* pGameObject, _In_ GeometryPtr& spGeometry, _In_ const L3DMath::Vector3& color);

        Renderer* _pRenderer;
        GDK::IGameObject* _pOwner;
        GeometryPtr _spQuad;
        L3DMath::Vector3 _color;
    };
}
