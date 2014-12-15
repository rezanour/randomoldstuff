#pragma once

// TODO: Move this elsewhere
inline XMFLOAT3 VecMin(_In_ const XMFLOAT3& a, _In_ const XMFLOAT3& b)
{
    return XMFLOAT3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

inline XMFLOAT3 VecMax(_In_ const XMFLOAT3& a, _In_ const XMFLOAT3& b)
{
    return XMFLOAT3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

inline XMFLOAT3 VecSub(_In_ const XMFLOAT3& a, _In_ const XMFLOAT3& b)
{
    return XMFLOAT3(b.x - a.x, b.y - a.y, b.z - a.z);
}

class AABB
{
public:
    AABB() : _min(0, 0, 0), _max(0, 0, 0)
    {}

    AABB(_In_ const AABB& other) : _min(other._min), _max(other._max)
    {}

    AABB(_In_ const XMFLOAT3& mins, _In_ const XMFLOAT3& maxs) : _min(mins), _max(maxs)
    {}

    const XMFLOAT3& GetMin() const { return _min; }
    const XMFLOAT3& GetMax() const { return _max; }

    void Add(_In_ const AABB& other)
    {
        _min = VecMin(_min, other._min);
        _max = VecMax(_max, other._max);
    }

    float GetVolume() const
    {
        XMFLOAT3 span(VecSub(_max, _min));
        return span.x * span.y * span.z;
    }

    bool Intersects(_In_ const AABB& other) const
    {
        return (_max.x >= other._min.x &&
                _max.y >= other._min.y &&
                _max.z >= other._min.z &&
                _min.x <= other._max.x &&
                _min.y <= other._max.y &&
                _min.z <= other._max.z);
    }

private:
    XMFLOAT3 _min;
    XMFLOAT3 _max;
};
