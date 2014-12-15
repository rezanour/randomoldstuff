#pragma once

class TexturePool;

enum class TextureType
{
    Diffuse = 0,
    Normal,
    Max
};

class Texture
{
public:
    Texture() : _pool(nullptr), _index(0)
    {}

    Texture(_In_ const std::shared_ptr<TexturePool>& pool, _In_ uint32_t index) : _pool(pool), _index(index)
    {
        assert(pool != nullptr);
    }

    // Allow copies
    Texture(const Texture& other)
        : _pool(other._pool), _index(other._index)
    {}

    // Allow moves
    Texture(Texture&& other)
        : _index(other._index)
    {
        _pool.swap(other._pool);
    }

    bool IsValid() const { return _pool != nullptr; }
    const std::shared_ptr<TexturePool>& GetPool() const { return _pool; }
    uint32_t GetIndex() const { return _index; }

private:
    std::shared_ptr<TexturePool> _pool;
    uint32_t _index;
};
