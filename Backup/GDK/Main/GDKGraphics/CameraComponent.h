#pragma once

#include <GDK\RefCounted.h>
#include <GDK\ObjectModel\ObjectComponent.h>

#define DIRECTXMATH_INTEROP
#include <Lucid3D.Math.h>

#include "GraphicsTypes.h"
#include <stde\non_copyable.h>

namespace Lucid3D
{
    class Renderer;

    class CameraComponent;
    typedef stde::ref_counted_ptr<CameraComponent> CameraPtr;

    class CameraComponent : stde::non_copyable, public GDK::RefCounted<GDK::ICameraComponent>
    {
    public:
        static HRESULT Create(  _In_ Renderer* pRenderer,
                                _In_ GDK::IGameObject* pGameObject, 
                                _In_ uint64 contentId, 
                                _Out_ CameraPtr& spComponent);

        // IObjectComponent
        GDK_IMETHOD_(const GDK::IGameObject*) GetOwner() const;
        GDK_IMETHOD_(GDK::ObjectComponentType) GetType() const;

        // Methods
        const Lucid3D::Matrix& GetViewMatrix()
        {
            if (GetOwner()->GetTransformSalt() != _transformSalt)
            {
                UpdateMatrices();
            }

            return _view;
        }

        const Lucid3D::Matrix& GetProjectionMatrix()
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
        Lucid3D::Matrix _view;
        Lucid3D::Matrix _projection;
    };
}
