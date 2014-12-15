#pragma once

#include "Platform.h"

namespace GDK
{
    class GameObject;
    typedef std::shared_ptr<GameObject> GameObjectPtr;

    class GameObject : public RuntimeObject<GameObject>
    {
    public:
        // Factory support
        typedef GameObjectPtr (*CreateMethod)(_In_ const std::wstring& type);

        static void RegisterFactory(_In_ const std::wstring& type, _In_ CreateMethod createMethod);
        static GameObjectPtr Create(_In_ const std::wstring& type);

        // GameObject
        const std::wstring& GetType() const;
        const std::wstring& GetId() const;

        const Vector3& GetPosition() const;
        float GetRotation() const;

        void SetPosition(_In_ const Vector3& position);
        void SetRotation(_In_ float rotation);

    protected:
        GameObject(_In_ const std::wstring& type, _In_ const std::wstring& id);

    private:
        std::wstring _type;
        std::wstring _id;
        Vector3 _position;
        float _rotation;

        static std::map<std::wstring, CreateMethod> _factories;
    };
}
