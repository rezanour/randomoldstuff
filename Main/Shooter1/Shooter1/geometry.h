#pragma once

class GeoPool;

class Geometry
{
public:
    Geometry() : _pool(nullptr), _verticesIndex(0), _indicesIndex(0), _indexCount(0)
    {}

    Geometry(_In_ const std::shared_ptr<GeoPool>& pool, _In_ uint32_t verticesIndex, _In_ uint32_t indicesIndex, _In_ uint32_t indexCount)
        : _pool(pool), _verticesIndex(verticesIndex), _indicesIndex(indicesIndex), _indexCount(indexCount)
    {
        assert(pool != nullptr);
    }

    // Allow copies
    Geometry(const Geometry& other)
        : _pool(other._pool), _verticesIndex(other._verticesIndex), _indicesIndex(other._indicesIndex), _indexCount(other._indexCount)
    {}

    // Allow moves
    Geometry(Geometry&& other)
        : _verticesIndex(other._verticesIndex), _indicesIndex(other._indicesIndex), _indexCount(other._indexCount)
    {
        _pool.swap(other._pool);
    }

    bool IsValid() const { return _pool != nullptr; }
    const std::shared_ptr<GeoPool>& GetPool() const { return _pool; }
    uint32_t GetVerticesIndex() const { return _verticesIndex; }
    uint32_t GetIndicesIndex() const { return _indicesIndex; }
    uint32_t GetIndexCount() const { return _indexCount; }

private:
    std::shared_ptr<GeoPool> _pool;
    uint32_t _verticesIndex;
    uint32_t _indicesIndex;
    uint32_t _indexCount;
};
