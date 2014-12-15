#include <GameObject.h>

namespace GDK
{
    std::map<std::wstring, GameObject::CreateMethod> GameObject::_factories;

    void GameObject::RegisterFactory(_In_ const std::wstring& type, _In_ CreateMethod createMethod)
    {
        _factories[type] = createMethod;
    }

    std::shared_ptr<GameObject> GameObject::Create(_In_ const std::wstring& type)
    {
        auto factory = _factories.find(type);
        if (factory == _factories.end())
        {
            throw std::invalid_argument("No factory registered for type.");
        }

        return factory->second(type);
    }

    GameObject::GameObject(_In_ const std::wstring& type, _In_ const std::wstring& id) :
        _type(type),
        _id(id),
        _position(),
        _rotation(0)
    {
    }

    const std::wstring& GameObject::GetType() const
    {
        return _type;
    }

    const std::wstring& GameObject::GetId() const
    {
        return _id;
    }

    const Vector3& GameObject::GetPosition() const
    {
        return _position;
    }

    float GameObject::GetRotation() const
    {
        return _rotation;
    }

    void GameObject::SetPosition(_In_ const Vector3& position)
    {
        _position = position;
    }

    void GameObject::SetRotation(_In_ float rotation)
    {
        _rotation = rotation;
    }
}
