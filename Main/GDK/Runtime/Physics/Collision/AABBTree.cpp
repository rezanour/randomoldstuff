#include <GDKError.h>
#include <CollisionPrimitives.h>
#include <Collision.h>
#include "AABBTree.h"

namespace GDK
{
    static void CenterBasedPartition(_In_ byte_t axis, _In_ const Vector3& mins, _In_ const Vector3& maxs, _In_ const std::vector<Triangle>& input, _Inout_ std::vector<Triangle>& left, _Inout_ std::vector<Triangle>& right)
    {
        left.clear();
        right.clear();

        float center = (mins[axis] + maxs[axis]) * 0.5f;

        for (auto i = 0; i < input.size(); ++i)
        {
            const Triangle& t = input[i];

            // use center of the aabb
            float minValue = std::min(std::min(t.a[axis], t.b[axis]), t.c[axis]);
            float maxValue = std::max(std::max(t.a[axis], t.b[axis]), t.c[axis]);
            float value = (minValue + maxValue) * 0.5f;

            if (value <= center)
            {
                left.push_back(t);
            }
            else
            {
                right.push_back(t);
            }
        }
    }

    static void MedianBasedPartition(_In_ byte_t axis, _In_ const std::vector<Triangle>& input, _Inout_ std::vector<Triangle>& left, _Inout_ std::vector<Triangle>& right)
    {
        left.clear();
        right.clear();

        std::vector<float> centroids(input.size());

        for (auto i = 0; i < input.size(); ++i)
        {
            const Triangle& t = input[i];
            centroids[i] = (1/3.0f) * (t.a[axis] + t.b[axis] + t.c[axis]);
        }

        std::sort(centroids.begin(), centroids.end());
        float median = centroids[centroids.size() / 2];

        for (auto i = 0; i < input.size(); ++i)
        {
            const Triangle& t = input[i];
            float value = (1/3.0f) * (t.a[axis] + t.b[axis] + t.c[axis]);

            if (value <= median)
            {
                left.push_back(t);
            }
            else
            {
                right.push_back(t);
            }
        }
    }

    static void ProcessNode(_In_ const std::vector<Triangle>& input, _Inout_ Triangle* triangles, _Inout_ AabbNode* nodes, _Inout_ uint32_t* nextTriangle, _Inout_ uint32_t* nextNode)
    {
        if (input.size() == 0)
        {
            // reached end of recursion
            return;
        }

        if (input.size() == 1)
        {
            // reached a leaf node

            const Triangle& triangle = input[0];

            AabbNode* node = &nodes[(*nextNode)++];
            node->left = AabbNode::LeafNode;
            node->right = *nextTriangle;
            triangles[(*nextTriangle)++] = triangle;

            node->mins = Vector3::Min(Vector3::Min(triangle.a, triangle.b), triangle.c);
            node->maxs = Vector3::Max(Vector3::Max(triangle.a, triangle.b), triangle.c);
            return;
        }

        // find AABB of the whole set of triangles
        Vector3 minCorner = input[0].a;
        Vector3 maxCorner = minCorner;

        for (auto i = 0; i < input.size(); ++i)
        {
            minCorner = Vector3::Min(minCorner, input[i].a);
            minCorner = Vector3::Min(minCorner, input[i].b);
            minCorner = Vector3::Min(minCorner, input[i].c);
            maxCorner = Vector3::Max(maxCorner, input[i].a);
            maxCorner = Vector3::Max(maxCorner, input[i].b);
            maxCorner = Vector3::Max(maxCorner, input[i].c);
        }

        // find longest axis, which is the preferred direction
        Vector3 diff = maxCorner - minCorner;
        std::vector<Triangle> left, right;

        byte_t axes[3];
        axes[0] = diff[0] > diff[1] ? (diff[0] > diff[2] ? 0 : 2) : (diff[1] > diff[2] ? 1 : 2);
        byte_t i2 = (axes[0] + 1) % 3;
        byte_t i3 = (axes[0] + 2) % 3;
        axes[1] = diff[i2] > diff[i3] ? i2 : i3;
        axes[2] = axes[1] == i2 ? i3 : i2;

        // for each possible axis, in order from best to worst, try center algorithm, then median if that fails
        for (auto i = 0; i < 3; ++i)
        {
            byte_t axis = axes[i];

            CenterBasedPartition(axis, minCorner, maxCorner, input, left, right);
            if (left.size() == 0 || right.size() == 0)
            {
                // fall back to median
                MedianBasedPartition(axis, input, left, right);
            }

            if (left.size() > 0 && right.size() > 0)
            {
                // good to go
                break;
            }
        }

        if (left.size() == 0 || right.size() == 0)
        {
            left.clear();
            right.clear();
            // after trying all 3 axes, and both partitioning algorithms, couldn't find a good split.
            // Just divide up the triangles in half as a worst case
            for (auto i = 0; i < input.size() / 2; ++i)
            {
                left.push_back(input[i]);
            }
            for (auto i = input.size() / 2; i < input.size(); ++i)
            {
                right.push_back(input[i]);
            }
        }

        // inner node
        AabbNode* node = &nodes[(*nextNode)++];

        node->left = *nextNode;
        ProcessNode(left, triangles, nodes, nextTriangle, nextNode);

        node->right = *nextNode;
        ProcessNode(right, triangles, nodes, nextTriangle, nextNode);

        node->mins = minCorner;
        node->maxs = maxCorner;
    }

    //===========================================================================================================

    static bool RayAabbTest(_In_ const Ray& ray, _In_ const Vector3 slopes[], _In_ const Vector3& mins, _In_ const Vector3& maxs)
    {
        Vector3 start = ray.start;
        Vector3 dir = ray.direction;

        // does the ray start inside the box? If so, it's a hit
        if (start.x > mins[0] && start.x < maxs[0] &&
            start.y > mins[1] && start.y < maxs[1] &&
            start.z > mins[2] && start.z < maxs[2])
        {
            return true;
        }

        // for each candidate side, find the point of intersection with that plane,
        // then test that the point is within the face.
        for (int i = 0; i < 3; ++i)
        {
            float startValue = start[i];
            float dirValue = dir[i];

            int i2 = (i + 1) % 3;
            int i3 = (i2 + 1) % 3;

            Vector3 p;
            bool hit = false;
            if (startValue < mins[i] && dirValue > 0)
            {
                p = start + slopes[i] * (mins[i] - startValue);
                hit = true;
            }
            else if (startValue > maxs[i] && dirValue < 0)
            {
                p = start + slopes[i] * (maxs[i] - startValue);
                hit = true;
            }

            if (hit)
            {
                float p2 = p[i2];
                float p3 = p[i3];
                if (p2 > mins[i2] && p2 < maxs[i2] && p3 > mins[i3] && p3 < maxs[i3])
                {
                    return true;
                }
            }
        }

        return false;
    }

    static bool AabbOverlap(_In_ const Vector3& mins1, _In_ const Vector3& maxs1, _In_ const Vector3& mins2, _In_ const Vector3& maxs2)
    {
        return (maxs2.x > mins1.x && maxs1.x > mins2.x &&
            maxs2.y > mins1.y && maxs1.y > mins2.y &&
            maxs2.z > mins1.z && maxs1.z > mins2.z);
    }

    //===========================================================================================================

    std::shared_ptr<AabbTree> AabbTree::Create(_Inout_ std::istream& input)
    {
        uint32_t numTriangles;
        input.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

        uint32_t numNodes = 2 * numTriangles - 1;

        std::unique_ptr<Triangle[]> triangles(CollisionPrimitive::CreateArray<Triangle>(numTriangles));
        std::unique_ptr<AabbNode[]> nodes(new AabbNode[numNodes]);

        input.read(reinterpret_cast<char*>(triangles.get()), sizeof(Triangle) * numTriangles);
        input.read(reinterpret_cast<char*>(nodes.get()), sizeof(AabbNode) * numNodes);

        return std::shared_ptr<AabbTree>(GDKNEW AabbTree(numTriangles, triangles, nodes));
    }

    std::shared_ptr<AabbTree> AabbTree::Create(_In_ const std::vector<Triangle>& triangles)
    {
        std::vector<Triangle> noDegenerates;
        for (auto i = 0; i < triangles.size(); ++i)
        {
            if (!triangles[i].IsDegenerate())
            {
                noDegenerates.push_back(triangles[i]);
            }
        }

        if (noDegenerates.size() == 0)
        {
            return nullptr;
        }

        uint32_t numTriangles = static_cast<uint32_t>(noDegenerates.size());
        uint32_t numNodes = 2 * numTriangles- 1;
        std::unique_ptr<Triangle[]> trianglesCopy(CollisionPrimitive::CreateArray<Triangle>(numTriangles));
        std::unique_ptr<AabbNode[]> nodes(new AabbNode[numNodes]);

        uint32_t nextNode = 0;
        uint32_t nextTriangle = 0;

        ProcessNode(noDegenerates, trianglesCopy.get(), nodes.get(), &nextTriangle, &nextNode);

        CHECK_TRUE(nextTriangle == numTriangles);
        CHECK_TRUE(nextNode == numNodes);

        return std::shared_ptr<AabbTree>(GDKNEW AabbTree(numTriangles, trianglesCopy, nodes));
    }

    AabbTree::AabbTree(_In_ uint32_t numTriangles, _Inout_ std::unique_ptr<Triangle[]>& triangles, _Inout_ std::unique_ptr<AabbNode[]>& nodes) :
        _numTriangles(numTriangles)
    {
        _triangles.swap(triangles);
        _nodes.swap(nodes);
    }

    SpacePartitionType AabbTree::GetType() const
    {
        return SpacePartitionType::AabbTree;
    }

    void AabbTree::Save(_Inout_ std::ostream& output) const
    {
        output.write(reinterpret_cast<const char*>(&_numTriangles), sizeof(_numTriangles));

        uint32_t numNodes = 2 * _numTriangles - 1;

        output.write(reinterpret_cast<char*>(_triangles.get()), sizeof(Triangle) * _numTriangles);
        output.write(reinterpret_cast<char*>(_nodes.get()), sizeof(AabbNode) * numNodes);
    }

    void AabbTree::GetAabb(_Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax) const
    {
        AabbNode* root = &_nodes[0];
        *aabbMin = Vector3((float)root->mins[0], (float)root->mins[1], (float)root->mins[2]);
        *aabbMax = Vector3((float)root->maxs[0], (float)root->maxs[1], (float)root->maxs[2]);
    }

    bool AabbTree::Raycast(_In_ const Ray& ray, _Out_opt_ RaycastResult* result) const
    {
        // Compute slope steppings
        Vector3 slopes[] = 
        {
            ray.direction * (1.0f / ray.direction.x),
            ray.direction * (1.0f / ray.direction.y),
            ray.direction * (1.0f / ray.direction.z),
        };
        return RaycastNode(0, ray, slopes, result);
    }

    bool AabbTree::Intersects(_In_ const CollisionPrimitive* primitive, _In_ CollideWithTriangle collisionCallback) const
    {
        Vector3 aabbMin, aabbMax;
        GetAabbForPrimitive(primitive, &aabbMin, &aabbMax);
        bool continueTesting = false;
        return IntersectsNode(0, aabbMin, aabbMax, primitive, collisionCallback, &continueTesting);
    }

    bool AabbTree::RaycastNode(_In_ uint32_t index, _In_ const Ray& ray, _In_ const Vector3 slopes[], _Out_opt_ RaycastResult* result) const
    {
        const AabbNode* node = &_nodes[index];

        if (RayAabbTest(ray, slopes, node->mins, node->maxs))
        {
            if (node->left == AabbNode::LeafNode)
            {
                return GDK::Raycast(&ray, nullptr, &_triangles[node->right], nullptr, result);
            }
            else
            {
                // if we want an exact result, we need to get the result of each child, and then take the 
                // closer of the two
                if (result)
                {
                    RaycastResult leftResult, rightResult;
                    bool leftHit = RaycastNode(node->left, ray, slopes, &leftResult);
                    bool rightHit = RaycastNode(node->right, ray, slopes, &rightResult);

                    if (!leftHit && !rightHit)
                    {
                        return false;
                    }
                    else if (leftHit && !rightHit)
                    {
                        *result = leftResult;
                        return true;
                    }
                    else if (!leftHit && rightHit)
                    {
                        *result = rightResult;
                        return true;
                    }
                    else
                    {
                        *result = (leftResult.distance < rightResult.distance) ? leftResult : rightResult;
                        return true;
                    }
                }
                else
                {
                    // in this case, we just care about hit or not
                    return RaycastNode(node->left, ray, slopes, nullptr) || RaycastNode(node->right, ray, slopes, nullptr);
                }
            }
        }

        return false;
    }

    bool AabbTree::IntersectsNode(_In_ uint32_t index, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax, _In_ const CollisionPrimitive* primitive, _In_ CollideWithTriangle collisionCallback, _Out_ bool* continueTesting) const
    {
        const AabbNode* node = &_nodes[index];

        if (AabbOverlap(aabbMin, aabbMax, node->mins, node->maxs))
        {
            if (node->left == AabbNode::LeafNode)
            {
                return collisionCallback(primitive, _triangles[node->right], continueTesting);
            }
            else
            {
                bool keepGoing = true, rightHit = false;
                bool leftHit = IntersectsNode(node->left, aabbMin, aabbMax, primitive, collisionCallback, &keepGoing);
                if (keepGoing)
                {
                    rightHit = IntersectsNode(node->right, aabbMin, aabbMax, primitive, collisionCallback, &keepGoing);
                }

                *continueTesting = keepGoing;
                return leftHit || rightHit;
            }
        }

        return false;
    }
}
