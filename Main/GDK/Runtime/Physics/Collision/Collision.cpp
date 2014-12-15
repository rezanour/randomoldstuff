#include <Collision.h>
#include <CollisionPrimitives.h>
#include <SpacePartition.h>
#include <GDKError.h>

namespace GDK
{
    //
    // All intersection tests fit the following format, so they can be put in the dispatch table.
    // The last parameter is optional, and tests should be optimized to only compute those 
    // more costly values when requested to do so.
    //
    typedef bool (*IntersectionTest)(
        _In_ const CollisionPrimitive* primA,
        _In_opt_ const Matrix* transformA,
        _In_ const CollisionPrimitive* primB,
        _In_opt_ const Matrix* transformB,
        _Inout_opt_ IntersectionResult* result);

    typedef bool (*RaycastTest)(
        _In_ const Ray* ray,
        _In_opt_ const Matrix* rayTransform,
        _In_ const CollisionPrimitive* prim,
        _In_opt_ const Matrix* primTransform,
        _Inout_opt_ RaycastResult* result);

    static std::map<uint16_t, IntersectionTest> g_intersectionDispatchTable;
    static std::map<CollisionPrimitiveType, RaycastTest> g_raycastDispatchTable;

    static uint16_t MakeKey(_In_ CollisionPrimitiveType typeA, _In_ CollisionPrimitiveType typeB)
    {
        return (static_cast<uint8_t>(typeA) << 8) | static_cast<uint16_t>(typeB);
    }

    // Forward declaration of primitive tests
    static bool SphereSphereTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    static bool SpherePlaneTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    static bool SphereTriangleTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    static bool AabbTriangleTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    static bool AlignedCapsuleTriangleTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    //static bool AlignedCapsuleTriangleMeshTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    static bool AlignedCapsuleAlignedCapsuleTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);
    //static bool SphereTriangleMeshTest(_In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ IntersectionResult*);

    // Forward declaration of raycast tests
    static bool RaycastSphere(_In_ const Ray*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ RaycastResult*);
    static bool RaycastPlane(_In_ const Ray*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ RaycastResult*);
    static bool RaycastTriangle(_In_ const Ray*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ RaycastResult*);
    static bool RaycastTriangleMesh(_In_ const Ray*, _In_opt_ const Matrix*, _In_ const CollisionPrimitive*, _In_opt_ const Matrix*, _Inout_opt_ RaycastResult*);

    namespace Physics
    {
        // Initializes the dispatch table. If you have a new intersection test to register,
        // this is the place to include it
        void Startup()
        {
            if (g_intersectionDispatchTable.size() == 0)
            {
                // NOTE: There is no need to create two versions of the same test, with swapped parameters, and register it twice.
                // When looking up the test, if it's not found, the look up logic flips the types and tries again, reversing the results
                // automatically
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::Sphere, CollisionPrimitiveType::Sphere)] = SphereSphereTest;
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::Sphere, CollisionPrimitiveType::Plane)] = SpherePlaneTest;
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::Sphere, CollisionPrimitiveType::Triangle)] = SphereTriangleTest;
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::Aabb, CollisionPrimitiveType::Triangle)] = AabbTriangleTest;
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::AlignedCapsule, CollisionPrimitiveType::Triangle)] = AlignedCapsuleTriangleTest;
      //          g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::AlignedCapsule, CollisionPrimitiveType::TriangleMesh)] = AlignedCapsuleTriangleMeshTest;
                g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::AlignedCapsule, CollisionPrimitiveType::AlignedCapsule)] = AlignedCapsuleAlignedCapsuleTest;
      //          g_intersectionDispatchTable[MakeKey(CollisionPrimitiveType::Sphere, CollisionPrimitiveType::TriangleMesh)] = SphereTriangleMeshTest;
            }

            if (g_raycastDispatchTable.size() == 0)
            {
                g_raycastDispatchTable[CollisionPrimitiveType::Sphere] = RaycastSphere;
                g_raycastDispatchTable[CollisionPrimitiveType::Plane] = RaycastPlane;
                g_raycastDispatchTable[CollisionPrimitiveType::Triangle] = RaycastTriangle;
                g_raycastDispatchTable[CollisionPrimitiveType::TriangleMesh] = RaycastTriangleMesh;
            }
        }

        void Shutdown()
        {
        }
    }

    bool Intersects(
        _In_ const CollisionPrimitive* primA,
        _In_opt_ const Matrix* transformA,
        _In_ const CollisionPrimitive* primB,
        _In_opt_ const Matrix* transformB,
        _Inout_opt_ IntersectionResult* result)
    {
        CHECK_NOT_NULL(primA);
        CHECK_NOT_NULL(primB);

        auto key = MakeKey(primA->type, primB->type);

        auto entry = g_intersectionDispatchTable.find(key);
        if (entry != g_intersectionDispatchTable.end())
        {
            return entry->second(primA, transformA, primB, transformB, result);
        }

        // If the key was not found, reverse the primitives and try again
        key = MakeKey(primB->type, primA->type);
        entry = g_intersectionDispatchTable.find(key);
        if (entry != g_intersectionDispatchTable.end())
        {
            // reverse the order of the primitives
            if (entry->second(primB, transformB, primA, transformA, result))
            {
                // if caller asked for results back, correct them for
                // the original order passed in
                if (result)
                {
                    result->normal = -result->normal;
                }
                return true;
            }
            return false;
        }

        // no test for the specified combination was found
        throw std::exception();
    }

    bool Raycast(
        _In_ const Ray* ray, 
        _In_opt_ const Matrix* rayTransform,
        _In_ const CollisionPrimitive* prim, 
        _In_opt_ const Matrix* primTransform,
        _Inout_opt_ RaycastResult* result)
    {
        CHECK_NOT_NULL(ray);
        CHECK_NOT_NULL(prim);

        auto entry = g_raycastDispatchTable.find(prim->type);
        if (entry != g_raycastDispatchTable.end())
        {
            return entry->second(ray, rayTransform, prim, primTransform, result);
        }

        // no raycast test found for this primitive
        throw std::exception();
    }

    // =================================================================================
    // Utility functions
    // =================================================================================

    // This function assumes that point is on the same plane as triangle
    static bool PointInTriangle(_In_ const Vector3& point, _In_ const Triangle* triangle)
    {
        // Using barycentric approach from my blog post.

        // NOTE: it might actually be faster to use edge-normals and dot those against p-vertex

        // Prepare barycentric variables
        Vector3 u = triangle->b - triangle->a;
        Vector3 v = triangle->c - triangle->a;
        Vector3 w = point - triangle->a;
        Vector3 vCrossW = Vector3::Cross(v, w);
        Vector3 vCrossU = Vector3::Cross(v, u);

        // Test sign of r
        if (Vector3::Dot(vCrossW, vCrossU) < 0)
            return false;

        Vector3 uCrossW = Vector3::Cross(u, w);
        Vector3 uCrossV = Vector3::Cross(u, v);

        // Test sign of t
        if (Vector3::Dot(uCrossW, uCrossV) < 0)
            return false;

        // At this point, we know that r & t are both > 0
        float denom = uCrossV.Length();
        float r = vCrossW.Length() / denom;
        float t = uCrossW.Length() / denom;

        return (r <= 1 && t <= 1 && r + t <= 1);
    }

    // This function assumes that point is on the same plane as triangle
    static Vector3 FindNearestPointOnTriangle(_In_ const Vector3& point, _In_ const Triangle* triangle)
    {
        Vector3 edges[] =
        {
            triangle->b - triangle->a,
            triangle->c - triangle->b,
            triangle->a - triangle->c
        };

        float edgeLengths[] =
        {
            edges[0].Length(),
            edges[1].Length(),
            edges[2].Length(),
        };

        edges[0].Normalize();
        edges[1].Normalize();
        edges[2].Normalize();

        Vector3 edgeNormals[] = 
        {
            Vector3::Cross(edges[0], triangle->normal),
            Vector3::Cross(edges[1], triangle->normal),
            Vector3::Cross(edges[2], triangle->normal),
        };

        Vector3 aToP = point - triangle->a;

        // Find distance to first edge of triangle
        float dist = Vector3::Dot(aToP, edgeNormals[0]);
        if (dist > 0) // Either in vertA, vertB, or edgeAB voronoi region
        {
            float r = Vector3::Dot(aToP, edges[0]);
            if (r > edgeLengths[0]) // vertB
            {
                return triangle->b;
            }
            else if (r < 0) // vertA
            {
                return triangle->a;
            }
            else // edgeAB
            {
                return triangle->a + edges[0] * r;
            }
        }
        else
        {
            Vector3 bToP = point - triangle->b;
            dist = Vector3::Dot(bToP, edgeNormals[1]);
            if (dist > 0) // Either in vertB, vertC, or edgeBC region
            {
                float r = Vector3::Dot(bToP, edges[1]);
                if (r > edgeLengths[1]) // vertC
                {
                    return triangle->c;
                }
                else if (r < 0) // vertB
                {
                    return triangle->b;
                }
                else // edgeBC
                {
                    return triangle->b + edges[1] * r;
                }
            }
            else
            {
                Vector3 cToP = point - triangle->c;
                dist = Vector3::Dot(cToP, edgeNormals[2]);
                if (dist > 0) // Either in vertA, vertC, or edgeCA region
                {
                    float r = Vector3::Dot(cToP, edges[2]);
                    if (r > edgeLengths[2]) // vertA
                    {
                        return triangle->a;
                    }
                    else if (r < 0) // vertC
                    {
                        return triangle->c;
                    }
                    else // edgeCA
                    {
                        return triangle->c + edges[2] * r;
                    }
                }
                else // inside of triangle, return info as is
                {
                    return point;
                }
            }
        }
    }

    static inline void GetAabbIntervalOnDirection(_In_ const Aabb* aabb, _In_ const Vector3& dir, _Out_ float* min, _Out_ float* max)
    {
        Vector3 center = (aabb->aabbMin + aabb->aabbMax) * 0.5f;
        Vector3 halfWidths = aabb->aabbMax - center;
        halfWidths.x *= sign(dir.x);
        halfWidths.y *= sign(dir.y);
        halfWidths.z *= sign(dir.z);
        Vector3 minPoint = center - halfWidths;
        Vector3 maxPoint = center + halfWidths;
        *min = Vector3::Dot(dir, minPoint);
        *max = Vector3::Dot(dir, maxPoint);
    }

    static inline void GetTriangleIntervalOnDirection(_In_ const Triangle* tri, _In_ const Vector3& dir, _Out_ float* min, _Out_ float* max)
    {
        Vector3 pts[] = { tri->a, tri->b, tri->c };
        *min = 10000;
        *max = -10000;

        for (auto i = 0; i < _countof(pts); ++i)
        {
            float dist = Vector3::Dot(pts[i], dir);
            if (dist < *min)
            {
                *min = dist;
            }
            if (dist > *max)
            {
                *max = dist;
            }
        }
    }

    // =================================================================================
    // Raycast tests
    // =================================================================================

    _Use_decl_annotations_
    bool RaycastPlane(const Ray* rayIn, const Matrix* rayTransform, const CollisionPrimitive* prim, const Matrix* primTransform, RaycastResult* result)
    {
        Ray ray;
        TransformPrimitive(rayIn, rayTransform, &ray);

        Plane plane;
        TransformPrimitive(prim, primTransform, &plane);

        float d = Vector3::Dot(ray.start, plane.normal);
        float distToPlane = plane.dist - d;
        float cosA = Vector3::Dot(ray.direction, plane.normal);
        if (GDK::sign(distToPlane) != GDK::sign(cosA))
        {
            return false;
        }

        if (result)
        {
            result->distance = distToPlane / cosA;
            result->point = ray.start + ray.direction * result->distance;
        }

        return true;
    }

    _Use_decl_annotations_
    bool RaycastSphere(const Ray* rayIn, const Matrix* rayTransform, const CollisionPrimitive* prim, const Matrix* primTransform, RaycastResult* result)
    {
        Ray ray;
        TransformPrimitive(rayIn, rayTransform, &ray);

        Sphere sphere;
        TransformPrimitive(prim, primTransform, &sphere);

        Vector3 toSphere = sphere.center - ray.start;
        float distFromSphereToRay = Vector3::Cross(ray.direction, toSphere).Length();
        if (distFromSphereToRay > sphere.radius)
        {
            return false;
        }

        if (result)
        {
            float distToSubtract = sqrtf((sphere.radius * sphere.radius) - (distFromSphereToRay * distFromSphereToRay));
            float totalDist = Vector3::Dot(ray.direction, toSphere);
            result->distance = totalDist - distToSubtract;
            result->point = ray.start + ray.direction * result->distance;
        }

        return true;
    }

    _Use_decl_annotations_
    bool RaycastTriangle(const Ray* rayIn, const Matrix* rayTransform, const CollisionPrimitive* prim, const Matrix* primTransform, RaycastResult* result)
    {
        Ray ray;
        TransformPrimitive(rayIn, rayTransform, &ray);

        Triangle triangle;
        TransformPrimitive(prim, primTransform, &triangle);

        // Is the ray start behind the plane of the triangle? If so, we can only
        // be hitting the triangle from behind, which we don't report, so bail out now
        if (Vector3::Dot(ray.start - triangle.a, triangle.normal) <= 0)
        {
            return false;
        }

        // does the ray intersect the plane of the triangle?
        Plane plane(triangle.normal, Vector3::Dot(triangle.a, triangle.normal));
        RaycastResult planeResult;
        if (!Raycast(&ray, nullptr, &plane, nullptr, &planeResult))
        {
            return false;
        }

        // if it intersected, is the point in the triangle?
        if (!PointInTriangle(planeResult.point, &triangle))
        {
            return false;
        }

        if (result)
        {
            *result = planeResult;
        }

        return true;
    }

    _Use_decl_annotations_
    bool RaycastTriangleMesh(const Ray* rayIn, const Matrix* rayTransform, const CollisionPrimitive* prim, const Matrix* primTransform, RaycastResult* result)
    {
        Ray ray;
        TransformPrimitive(rayIn, rayTransform, &ray);

        TriangleMesh mesh;
        TransformPrimitive(prim, primTransform, &mesh);

        // convert ray into local space of the BVH
        Ray transformedRay;
        TransformPrimitive(&ray, &mesh.invTransform, &transformedRay);

        return mesh.data->Raycast(transformedRay, result);
    }

    // =================================================================================
    // Intersection Tests
    // =================================================================================

    _Use_decl_annotations_
    bool SphereSphereTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        Sphere a, b;
        TransformPrimitive(primA, transformA, &a);
        TransformPrimitive(primB, transformB, &b);

        Vector3 dir = a.center - b.center;
        float distSquared = dir.LengthSquared();
        float r = a.radius + b.radius;
        if (distSquared > r * r)
        {
            return false;
        }

        if (result)
        {
            result->normal = Vector3::Normalize(dir);
            result->depth = r - sqrtf(distSquared);
        }

        return true;
    }

    _Use_decl_annotations_
    bool SpherePlaneTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        Sphere sphere;
        TransformPrimitive(primA, transformA, &sphere);

        Plane plane;
        TransformPrimitive(primB, transformB, &plane);

        float distToPlane = Vector3::Dot(sphere.center, plane.normal) - plane.dist;
        if (fabsf(distToPlane) > sphere.radius)
        {
            return false;
        }

        if (result)
        {
            result->normal = plane.normal * sign(distToPlane);
            result->depth = sphere.radius - distToPlane;
        }

        return true;
    }

    _Use_decl_annotations_
    bool SphereTriangleTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        Sphere sphere;
        TransformPrimitive(primA, transformA, &sphere);

        Triangle tri;
        TransformPrimitive(primB, transformB, &tri);

        // does sphere even intersect the plane of the triangle?
        float distToPlane = Vector3::Dot(sphere.center - tri.a, tri.normal);
        if (fabsf(distToPlane) > sphere.radius)
        {
            return false;
        }

        Vector3 p = sphere.center - tri.normal * distToPlane;
        Vector3 nearestPointOnTriangle = FindNearestPointOnTriangle(p, &tri);

        Vector3 toCenter = sphere.center - nearestPointOnTriangle;

        // does the sphere intersect the triangle?
        if (toCenter.LengthSquared() > sphere.radius * sphere.radius)
        {
            return false;
        }

        if (result)
        {
            result->normal = Vector3::Normalize(toCenter);
            result->depth = sphere.radius - toCenter.Length();
        }

        return true;
    }

    _Use_decl_annotations_
    bool AabbTriangleTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        Aabb aabb;
        TransformPrimitive(primA, transformA, &aabb);

        Triangle tri;
        TransformPrimitive(primB, transformB, &tri);

        Vector3 axes[] = 
        {
            Vector3::Right(),
            Vector3::Up(),
            Vector3::Forward(),
            tri.normal,
            Vector3::Normalize(Vector3::Cross(Vector3::Right(), tri.b - tri.a)),
            Vector3::Normalize(Vector3::Cross(Vector3::Right(), tri.c - tri.b)),
            Vector3::Normalize(Vector3::Cross(Vector3::Right(), tri.a - tri.c)),
            Vector3::Normalize(Vector3::Cross(Vector3::Up(), tri.b - tri.a)),
            Vector3::Normalize(Vector3::Cross(Vector3::Up(), tri.c - tri.b)),
            Vector3::Normalize(Vector3::Cross(Vector3::Up(), tri.a - tri.c)),
            Vector3::Normalize(Vector3::Cross(Vector3::Forward(), tri.b - tri.a)),
            Vector3::Normalize(Vector3::Cross(Vector3::Forward(), tri.c - tri.b)),
            Vector3::Normalize(Vector3::Cross(Vector3::Forward(), tri.a - tri.c)),
        };

        float min1, min2, max1, max2;

        if (result)
        {
            result->depth = 10000;
        }

        for (auto i = 0; i < _countof(axes); ++i)
        {
            // find min and max of each object along axis
            GetAabbIntervalOnDirection(&aabb, axes[i], &min1, &max1);
            GetTriangleIntervalOnDirection(&tri, axes[i], &min2, &max2);

            float d1 = max1 - min2;
            if (d1 <= 0)
            {
                return false;
            }

            float d2 = max2 - min1;
            if (d2 <= 0)
            {
                return false;
            }

            if (result)
            {
                float sign = ((d1 < d2) ? -1.0f : 1.0f);
                float depth = std::min(d1, d2);
                if (depth < result->depth)
                {
                    result->depth = depth;
                    result->normal = axes[i] * sign;
                }
            }
        }

        return true;
    }

    _Use_decl_annotations_
    bool AlignedCapsuleTriangleTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        AlignedCapsule capsule;
        TransformPrimitive(primA, transformA, &capsule);

        Triangle tri;
        TransformPrimitive(primB, transformB, &tri);

        // Find the closest point on the line segment in the capsule to the plane of the triangle
        Vector3 p1 = capsule.center + Vector3::Up() * (capsule.length * 0.5f);
        Vector3 p2 = p1 + Vector3::Down() * capsule.length;

        float d1 = Vector3::Dot(p1 - tri.a, tri.normal);
        float d2 = Vector3::Dot(p2 - tri.a, tri.normal);

        Vector3 closestPoint;

        if (d1 * d2 < 0) // opposite sides of the plane, so we intersected it
        {
            closestPoint = p1 + Vector3::Down() * (fabsf(d1) / (fabsf(d1) + fabsf(d2))) * capsule.length;
        }
        else
        {
            d1 = fabsf(d1);
            d2 = fabsf(d2);

            if (std::min(d1, d2) > capsule.radius)
            {
                return false;
            }

            closestPoint = (d1 <= d2) ? p1 : p2;
        }

        // Find projection of this point on the plane, then find the nearest point on the triangle
        float distToPlane = Vector3::Dot(closestPoint - tri.a, tri.normal);
        Vector3 pt = closestPoint - tri.normal * distToPlane;

        pt = FindNearestPointOnTriangle(pt, &tri);

        Vector3 toSeg = closestPoint - pt;
        if (toSeg.LengthSquared() > capsule.radius * capsule.radius)
        {
            return false;
        }

        if (result)
        {
            result->normal = Vector3::Normalize(toSeg);
            result->depth = capsule.radius - toSeg.Length();
        }

        return true;
    }

#if 0
    _Use_decl_annotations_
    bool AlignedCapsuleTriangleMeshTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        TriangleMesh triMesh;
        TransformPrimitive(primB, transformB, &triMesh);

        Matrix trans = (transformA) ? *transformA : Matrix::Identity();
        trans = trans * triMesh.invTransform;

        AlignedCapsule capsule;
        TransformPrimitive(primA, &trans, &capsule);

        return triMesh.data->Intersects(
            &capsule, 
            [](const CollisionPrimitive* primitive, const Triangle& triangle, IntersectionResult* result, bool* continueTesting) -> bool
        {

        });

        static std::vector<Triangle*> triangles(100);
        triangles.clear();

        Vector3 aabbMin, aabbMax;
        GetAabbForPrimitive(&capsule, &aabbMin, &aabbMax);
        triMesh.bvh->QueryAabb(aabbMin, aabbMax, triangles);

        IntersectionResult next, closest;
        bool hit = false;
        const Triangle* const * t = triangles.data();
        int32_t count = static_cast<int32_t>(triangles.size());
        while (count-- > 0)
        {
            if (Intersects(&capsule, nullptr, *t, nullptr, &next))
            {
                if (!result)
                {
                    return true;
                }

                if (!hit || next.depth < closest.depth)
                {
                    closest = next;
                    hit = true;
                }
            }
            ++t;
        }
        if (hit && result)
        {
            // transform result back into world coordinates from local trimesh coordinates
            Matrix::Inverse(triMesh.invTransform, &trans);
            result->normal = Matrix::TransformNormal(result->normal, trans);
        }
        return hit;
    }
#endif

    _Use_decl_annotations_
    bool AlignedCapsuleAlignedCapsuleTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        AlignedCapsule capsule1;
        TransformPrimitive(primA, transformA, &capsule1);

        AlignedCapsule capsule2;
        TransformPrimitive(primB, transformB, &capsule2);

        // Exploit the fact that these are upright aligned capsules
        Vector3 a1 = capsule1.center + Vector3::Up() * (capsule1.length * 0.5f);
        Vector3 a2 = a1 + Vector3::Down() * capsule1.length;
        Vector3 b1 = capsule2.center + Vector3::Up() * (capsule2.length * 0.5f);
        Vector3 b2 = b1 + Vector3::Down() * capsule2.length;

        Vector3 aClosest, bClosest;
        // find overlap of the two intervals on y
        if (b1.y < a2.y)
        {
            aClosest = a2;
            bClosest = b1;
        }
        else if (b2.y > a1.y)
        {
            aClosest = a1;
            bClosest = b2;
        }
        else
        {
            // inner points
            float y = b1.y;
            if (y > a2.y + capsule1.length)
            {
                y = a2.y + capsule1.length;
            }

            aClosest = a1;
            bClosest = b1;
            aClosest.y = bClosest.y = y;
        }

        Vector3 dist = aClosest - bClosest;
        float r = capsule1.radius + capsule2.radius;
        if (dist.LengthSquared() > r * r)
        {
            return false;
        }

        if (result)
        {
            result->depth = r - dist.Length();
            result->normal = Vector3::Normalize(dist);
        }

        return true;
    }

#if 0
    _Use_decl_annotations_
    bool SphereTriangleMeshTest(const CollisionPrimitive* primA, const Matrix* transformA, const CollisionPrimitive* primB, const Matrix* transformB, IntersectionResult* result)
    {
        Sphere sphere;
        TransformPrimitive(primA, transformA, &sphere);

        TriangleMesh triMesh;
        TransformPrimitive(primB, transformB, &triMesh);

        // convert sphere into local space of the BVH
        Sphere transformedSphere;
        TransformPrimitive(&sphere, &triMesh.invTransform, &transformedSphere);

        static std::vector<Triangle*> triangles(100);
        triangles.clear();

        Vector3 aabbMin, aabbMax;
        GetAabbForPrimitive(&transformedSphere, &aabbMin, &aabbMax);
        triMesh.bvh->QueryAabb(aabbMin, aabbMax, triangles);

        IntersectionResult next, closest;
        bool hit = false;
        const Triangle* const * t = triangles.data();
        int32_t count = static_cast<int32_t>(triangles.size());
        while (count-- > 0)
        {
            if (Intersects(&transformedSphere, nullptr, *t, nullptr, &next))
            {
                if (!result)
                {
                    return true;
                }

                if (!hit || next.depth < closest.depth)
                {
                    closest = next;
                    hit = true;
                }
            }
            ++t;
        }
        if (hit && result)
        {
            // transform result back into world coordinates from local trimesh coordinates
            Matrix trans;
            Matrix::Inverse(triMesh.invTransform, &trans);
            result->normal = Matrix::TransformNormal(result->normal, trans);
        }
        return hit;
    }
#endif
}
