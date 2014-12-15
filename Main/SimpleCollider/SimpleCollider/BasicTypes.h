#pragma once

template <class Derived> // template parameter is just for better error messages
class NonCopyable
{
public:
    NonCopyable()
    {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator= (const NonCopyable&);
};

class AxisAlignedBox
{
public:
    AxisAlignedBox() :
        _min(0, 0, 0), _max(0, 0, 0)
    {}

    AxisAlignedBox(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max) :
        _min(min), _max(max)
    {}

    const DirectX::XMFLOAT3& Min() const
    {
        return _min;
    }

    const DirectX::XMFLOAT3& Max() const
    {
        return _max;
    }

private:
    DirectX::XMFLOAT3 _min;
    DirectX::XMFLOAT3 _max;
};
