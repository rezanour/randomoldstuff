#pragma once

enum class ColliderType
{
    Static = 0,
    Dynamic
};

//
// Forward declares
//
struct ICollider;
struct IStaticCollider;
struct IDynamicCollider;

//
// Base collider interface
//
struct ICollider
{
    virtual ~ICollider()
    {}

    //
    // Get properties
    //
    virtual ColliderType Type() const = 0;
    virtual const ::Transform& Transform() const = 0;
    virtual AxisAlignedBox Bounds() const = 0;

    //
    // Modifiable
    //
    virtual ::Transform& Transform() = 0;

    //
    // Queries
    //
    virtual bool Raycast(DirectX::FXMVECTOR start, DirectX::FXMVECTOR direction, _Out_ float* distance) const = 0;
    virtual bool Collide(const IDynamicCollider* object, DirectX::FXMVECTOR movement, _Out_ DirectX::XMFLOAT3* contactPosition, _Out_ DirectX::XMFLOAT3* contactNormal, _Out_ float* distance) const = 0;
};

//
// Static collider objects
//
struct IStaticCollider : ICollider
{
    virtual ~IStaticCollider()
    {}
};

struct IStaticColliderBuilder
{
    virtual void __vectorcall AddVertices(DirectX::FXMMATRIX transform, const void* positions, uint32_t numVertices, uint32_t stride, _In_opt_count_(numIndices) const uint32_t* indices, uint32_t numIndices) = 0;
    virtual std::shared_ptr<IStaticCollider> Commit() = 0;
};

std::shared_ptr<IStaticColliderBuilder> CreateStaticColliderBuilder();

//
// Dynamic collider objects
//
struct IDynamicCollider : ICollider
{
    virtual ~IDynamicCollider()
    {}

    virtual const std::shared_ptr<Shape>& Shape() const = 0;
    virtual void SetShape(const std::shared_ptr<::Shape>& shape) = 0;
};

std::shared_ptr<IDynamicCollider> CreateDynamicCollider();
