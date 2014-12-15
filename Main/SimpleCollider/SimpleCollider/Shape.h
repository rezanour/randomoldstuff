#pragma once

enum class ShapeType
{
    Sphere = 0,
    Box,
    Cylinder
};

//
// Base shape type
//
class Shape : NonCopyable<Shape>
{
public:
    virtual ~Shape()
    {}

    ShapeType Type() const
    {
        return _type;
    }

protected:
    Shape(ShapeType type) :
        _type(type)
    {}

private:
    ShapeType _type;
};

//
// Sphere shape
//
class SphereShape : NonCopyable<SphereShape>, public Shape
{
public:
    SphereShape(const DirectX::XMFLOAT3& offset, float radius) :
        Shape(ShapeType::Sphere), _offset(offset), _radius(radius)
    {}

    const DirectX::XMFLOAT3& Offset() const
    {
        return _offset;
    }

    float Radius() const
    {
        return _radius;
    }

private:
    DirectX::XMFLOAT3 _offset;
    float _radius;
};
