#pragma once

#include <GDK\RefCounted.h>
#include <GDK\ObjectModel\ObjectComponent.h>

#define DIRECTXMATH_INTEROP
#include <L3DMath\L3DMath.h>

#include "GraphicsTypes.h"
#include <stde\non_copyable.h>

namespace Lucid3D
{
    class Renderer;

    class CameraComponent;
    typedef stde::ref_counted_ptr<CameraComponent> CameraPtr;

    class CameraComponent : stde::non_copyable, public GDK::RefCountedBase<GDK::ICameraComponent>
    {
    public:
        static HRESULT Create(  _In_ Renderer* pRenderer,
                                _In_ GDK::IGameObject* pGameObject, 
                                _In_ uint64 contentId, 
                                _Out_ CameraPtr& spComponent);

        // IObjectComponent
        GDK_METHOD_(const GDK::IGameObject*) GetOwner() const;
        GDK_METHOD_(GDK::ObjectComponentType) GetType() const;

        // Methods
        const L3DMath::Matrix& GetViewMatrix()
        {
            if (GetOwner()->GetTransformSalt() != _transformSalt)
            {
                UpdateMatrices();
            }

            return _view;
        }

        const L3DMath::Matrix& GetProjectionMatrix()
        {
            return _projection;
        }

        const float GetNearClipPlane() const
        {
            return _nearClipPlane;
        }

        const float GetFarClipPlane() const
        {
            return _farClipPlane;
        }

    private:
        CameraComponent(_In_ GDK::IGameObject* pGameObject, _In_ float fov, _In_ float aspectRatio, _In_ float nearClip, _In_ float farClip);

        void UpdateMatrices();

        GDK::IGameObject* _pOwner;
        float _fov;
        float _aspectRatio;
        float _nearClipPlane;
        float _farClipPlane;
        size_t _transformSalt;
        L3DMath::Matrix _view;
        L3DMath::Matrix _projection;
    };
}
