#pragma once

template <typename Interface>
class Collider : public Interface
{
public:
    virtual ~Collider()
    {}

    ColliderType Type() const override
    {
        return _type;
    }

    const ::Transform& Transform() const override
    {
        return _transform;
    }

    ::Transform& Transform() override
    {
        return _transform;
    }

protected:
    Collider(ColliderType type) :
        _type(type)
    {}

private:
    ::Transform _transform;
    ColliderType _type;
};

//
// Static collider
//
class StaticCollider : NonCopyable<StaticCollider>, public Collider<IStaticCollider>
{
public:
    StaticCollider() :
        Collider(ColliderType::Static)
    {}

    AxisAlignedBox Bounds() const override;

    bool Raycast(FXMVECTOR start, FXMVECTOR direction, _Out_ float* distance) const override;
    bool Collide(const IDynamicCollider* object, FXMVECTOR movement, _Out_ XMFLOAT3* contactPosition, _Out_ XMFLOAT3* contactNormal, _Out_ float* distance) const override;

private:
};

//
// Dynamic collider
//
class DynamicCollider : NonCopyable<DynamicCollider>, public Collider<IDynamicCollider>
{
public:
    DynamicCollider() :
        Collider(ColliderType::Dynamic)
    {}

    const std::shared_ptr<::Shape>& Shape() const override
    {
        return _shape;
    }

    void SetShape(const std::shared_ptr<::Shape>& shape) override
    {
        _shape = shape;
    }

    AxisAlignedBox Bounds() const override;

    bool Raycast(FXMVECTOR start, FXMVECTOR direction, _Out_ float* distance) const override;
    bool Collide(const IDynamicCollider* object, FXMVECTOR movement, _Out_ XMFLOAT3* contactPosition, _Out_ XMFLOAT3* contactNormal, _Out_ float* distance) const override;

private:
    std::shared_ptr<::Shape> _shape;
};
