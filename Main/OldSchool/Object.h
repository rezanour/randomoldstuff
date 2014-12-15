#pragma once

struct __declspec(novtable) IReadonlyObject
{
    virtual const Transform& GetTransform() const = 0;
    virtual AABB GetAABB() const = 0;

    virtual uint32_t RegisterForAABBChangeNotifications(_In_ const std::function<void()>& callback) = 0;
    virtual void UnregisterFromAABBChangeNotifications(_In_ uint32_t token) = 0;
};

struct __declspec(novtable) IWriteableObject : IReadonlyObject
{
    virtual void SetTransform(_In_ const Transform& transform) = 0;

    virtual void SendAABBChangeNotifications() = 0;
};

//
// Simple implementation of a readonly object. This is useful when you
// don't want to create a full entity for something that never changes.
//
class ReadonlyObject : public BaseObject<ReadonlyObject>, public IReadonlyObject
{
public:
    static std::shared_ptr<ReadonlyObject> Create(_In_ const Transform& transform, _In_ const AABB& aabb)
    {
        return std::shared_ptr<ReadonlyObject>(new ReadonlyObject(transform, aabb));
    }

private:
    const Transform& GetTransform() const override
    {
        return _transform;
    }

    AABB GetAABB() const override
    {
        return _aabb;
    }

    uint32_t RegisterForAABBChangeNotifications(_In_ const std::function<void()>& callback) override
    {
        UNREFERENCED_PARAMETER(callback);
        return 0;
    }

    void UnregisterFromAABBChangeNotifications(_In_ uint32_t token) override
    {
        UNREFERENCED_PARAMETER(token);
    }

private:
    ReadonlyObject(_In_ const Transform& transform, _In_ const AABB& aabb) :
        _transform(transform), _aabb(aabb)
    {
    }

private:
    Transform _transform;
    AABB _aabb;
};
