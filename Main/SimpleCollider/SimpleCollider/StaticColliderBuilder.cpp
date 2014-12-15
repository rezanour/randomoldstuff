#include "Precomp.h"
#include "SimpleCollider.h"
#include "Collider_p.h"

using namespace DirectX;

class StaticColliderBuilder : NonCopyable<StaticColliderBuilder>, public IStaticColliderBuilder
{
public:
    void __vectorcall AddVertices(DirectX::FXMMATRIX transform, const void* positions, uint32_t numVertices, uint32_t stride, _In_opt_count_(numIndices) const uint32_t* indices, uint32_t numIndices) override
    {
        UNREFERENCED_PARAMETER(transform);
        UNREFERENCED_PARAMETER(indices);
        UNREFERENCED_PARAMETER(numIndices);

        const uint8_t* p = (const uint8_t*)positions;
        for (uint32_t i = 0; i < numVertices; ++i)
        {
            _positions.push_back(*(XMFLOAT3*)p);
            p += stride;
        }
    }

    std::shared_ptr<IStaticCollider> Commit() override
    {
        return std::shared_ptr<IStaticCollider>(new StaticCollider());
    }

private:
    std::vector<XMFLOAT3> _positions;
};

std::shared_ptr<IStaticColliderBuilder> CreateStaticColliderBuilder()
{
    return std::shared_ptr<IStaticColliderBuilder>(new StaticColliderBuilder);
}
