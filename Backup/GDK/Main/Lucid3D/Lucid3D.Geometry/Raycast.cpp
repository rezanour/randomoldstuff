#include "StdAfx.h"
#include "Raycast.h"
#include "Ray.h"
#include "Plane.h"
#include "AABB.h"
#include "Sphere.h"

using namespace Lucid3D;
using Lucid3D::Geometry::AABB;
using Lucid3D::Geometry::Sphere;
using Lucid3D::Geometry::Ray;
using Lucid3D::Geometry::Plane;

namespace Lucid3D
{
    namespace Geometry
    {
        bool Raycast(_In_ const Ray& ray, _In_ const Plane& plane, _Out_opt_ float* distance)
        {
            if (Vector3::Dot(ray.GetDirection(), plane.GetNormal()) > 0.0f)
                return false;

            float dist = Vector3::Dot(plane.GetNormal(), ray.GetStart() - (plane.GetNormal() * plane.GetDist()));
            float cosTheta = Vector3::Dot(-plane.GetNormal(), ray.GetDirection());
            if (abs(cosTheta) > Math::Epsilon)
            {
                float hyp = dist / cosTheta;
                dist = hyp;
            }

            if (distance)
                *distance = dist;

            return true;
        }

        bool Raycast(_In_ const Ray& ray, _In_ const Sphere& sphere, _Out_opt_ float* distance)
        {
            Vector3 sphereToStart = ray.GetStart() - sphere.GetCenter();
            float r = sphere.GetRadius();

            // Consider sphereToStart as the normal of a plane through the center of the sphere.
            // If the point where the ray intersects this plane is further away than radius from the center, 
            // then we don't have an intersection
            float dist = sphereToStart.Length();
            float cosTheta = Vector3::Dot(Vector3::Normalize(sphereToStart), ray.GetDirection());

            // if ray is pointing directly at sphere, it intersects. So only try to rule out if it's not
            if (abs(cosTheta) > Math::Epsilon)
            {
                float hyp = dist / cosTheta;
                float opp = sqrt(hyp*hyp - dist*dist);
                if (opp > r)
                {
                    return false;
                }

                // TODO: compute real distance to sphere
            }

            if (distance)
                *distance = dist;

            return true;
        }

        bool Raycast(_In_ const Ray& ray, _In_ const AABB& box, _Out_opt_ float* distance)
        {
            Vector3 center = box.GetCenter();
            Vector3 halfWidths = box.GetHalfWidths();

            float dist;
            Vector3 pt;

            // check each face of the aabb

            Vector3 normals[] = { Vector3::Up(), Vector3::Down(), Vector3::Left(), Vector3::Right(), Vector3::Forward(), Vector3::Backward() };
            for (size_t i = 0; i < _countof(normals); i++)
            {
                pt = center + normals[i] * Vector3::Dot(normals[i], halfWidths);
                if (Raycast(ray, Plane(pt, normals[i]), &dist))
                {
                    pt = ray.GetStart() + ray.GetDirection() * dist;
                    pt = pt - center;
                    if (abs(pt.x) < halfWidths.x &&
                        abs(pt.y) < halfWidths.y &&
                        abs(pt.z) < halfWidths.z)
                    {
                        if (distance)
                            *distance = dist;

                        return true;
                    }
                }
            }

            return false;
        }
    }
}
