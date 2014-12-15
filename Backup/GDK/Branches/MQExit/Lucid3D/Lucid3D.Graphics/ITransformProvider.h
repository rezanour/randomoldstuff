// Implemented by clients, this lets graphics objects poll for transform data
// on demand
#pragma once

namespace Lucid3D
{
    namespace Graphics
    {
        struct ITransformProvider : public IRefCounted
        {
            virtual void __stdcall GetTransform(_Out_ Matrix* matrix) const = 0;
        };
    }
}
