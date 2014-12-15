#pragma once

#include "..\GDKTypes.h"

namespace GDK
{
    class GameObject;
    typedef stde::ref_counted_ptr<GameObject> GameObjectPtr;

    class Screen;

    class GameObject : stde::non_copyable, public GDK::RefCountedBase<GDK::IGameObject>
    {
    public:
        ~GameObject();

        static HRESULT Create(_In_ const std::string& name, _Out_ GameObjectPtr& spGameObject);
        static HRESULT Load(_In_ Screen* pScreen, _In_ uint64 contentId, _Out_ GameObjectPtr& spGameObject);

        // IGameObject
        GDK_IMETHOD_(unsigned long) GetID() const;
        GDK_IMETHOD_(const char* const) GetName() const;
        GDK_IMETHOD_(void) SetName(_In_ const char* name);

        GDK_IMETHOD_(size_t) GetTransformSalt() const;
        GDK_IMETHOD_(void) GetTransform(_Out_ GDK::Transform* pTransform) const;
        GDK_IMETHOD_(void) SetTransform(_In_ const GDK::Transform& transform);

        GDK_IMETHOD GetPropertyBag(_Deref_out_ GDK::IPropertyBag** ppProperties) const;

    private:
        explicit GameObject(_In_ const std::string& name);

        void UpdateMatrix();

        ulong _id;
        std::string _name;

        size_t _transformSalt;
        GDK::Transform _transform;

        stde::ref_counted_ptr<GDK::IPropertyBag> _spPropertyBag;

        // Currently no need to expose these, even internally
        std::vector<IObjectComponentPtr> _components;
    };
}
