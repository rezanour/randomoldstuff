#include <SpacePartition.h>
#include <CollisionPrimitives.h>
#include "AABBTree.h"

namespace GDK
{
    namespace SpacePartition
    {
        std::shared_ptr<ISpacePartition> Create(_Inout_ std::istream& input)
        {
            SpacePartitionType type;
            input.read(reinterpret_cast<char*>(&type), sizeof(type));

            switch (type)
            {
            case SpacePartitionType::AabbTree:
                return AabbTree::Create(input);

            //case SpacePartitionType::BihTree:
                //return BihTree::Create(input);

            default:
                throw std::invalid_argument("type");
            }
        }

        std::shared_ptr<ISpacePartition> Create(_In_ SpacePartitionType type, _In_ const std::vector<Triangle>& triangles)
        {
            switch (type)
            {
            case SpacePartitionType::AabbTree:
                return AabbTree::Create(triangles);

            //case SpacePartitionType::BihTree:
                //return BihTree::Create(triangles);

            default:
                throw std::invalid_argument("type");
            }
        }
    }
}
