#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_CAMERACOMPONENT_H_
#define _GDK_CAMERACOMPONENT_H_

////////////////////////////////////////////////////////////////////////////////
// camera component which allows scene rendering from this perspective

namespace GDK
{
    struct IGameObject;
    class Component;

    enum CameraType
    {
        CameraType_Perspective = 0,
        CameraType_Orthographic
    };

    class CameraComponent : public Component
    {
    public:
        static HRESULT Create(_In_ Object* owner, _In_ CameraType type, _Out_ CameraComponent** camera);

    private:
        CameraComponent(_In_ Object* owner, _In_ CameraType type);
        ~CameraComponent();

        const CameraType _type;
        void* _viewMatrix;
        void* _projMatrix;
    };

} // GDK

#endif // _GDK_CAMERACOMPONENT_H_
