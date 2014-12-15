#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// Enable VECTORCALL for DirectXMath if OSM is using it
#if defined(_OSM_VECTORCALL_)
#ifndef _XM_VECTORCALL_
#define _XM_VECTORCALL_ 1
#endif
#endif // defined(_OSM_VECTORCALL_)

#include <DirectXMath.h>

// Set OSM calling convention
#if defined(_OSM_VECTORCALL_)
#define OSMCALL __vectorcall
#else
#define OSMCALL // Don't define as anything, let compiler pick
#endif // _OSM_VECTORCALL_

// Set up typedefs for function parameters
#if defined(_OSM_VECTORCALL_)
typedef const DirectX::XMFLOAT3& OSMFLOAT3;
typedef const DirectX::XMFLOAT4& OSMFLOAT4;
#else
typedef const DirectX::XMFLOAT3& OSMFLOAT3;
typedef const DirectX::XMFLOAT4& OSMFLOAT4;
#endif // _OSM_VECTORCALL_

#if !defined(_XM_NO_INTRINSICS_)
#define OSMALIGN16 __declspec(align(16))
#else
#define OSMALIGN16
#endif

// Convenience defines
#define OSMINTERFACE struct __declspec(novtable)

//
// Common types
//

OSMALIGN16
struct RaycastResult
{
    DirectX::XMVECTOR Normal;
    float Lambda;
};

OSMALIGN16
struct OverlapResult
{
    DirectX::XMVECTOR Normal;
    float Depth;
};

OSMALIGN16
struct SweptResult
{
    DirectX::XMVECTOR Normal;
    float TOI;
};


//
// OSM headers
//

// Core headers
#include <AABB.h>
//#include <Gjk.h>

// Shapes
//#include <Shape.h>
//#include <ConvexShape.h>
//#include <Box.h>
//#include <Sphere.h>
//#include <Minkowski.h>

// Queries
//#include <Raycast.h>
//#include <Overlap.h>
//#include <Swept.h>

#include <SimpleCollision.h>
