// Transform is a structure containing all the individual components of a transform, such as position, orientation, and scale.
#pragma once

#include "Platform.h"
#include <DirectXTK\DirectXMath.h>

namespace GDK
{
    struct Transform
    {
        // The translational position of the object
        DirectX::XMFLOAT3 Position;
        // The scale factor in each axis direction
        DirectX::XMFLOAT3 Scale;
        // The orientation of the object represented as a unit quaternion
        DirectX::XMFLOAT4 Orientation;
        // Cached matrix representation which is valid when you receive the transform object.
        // Any manipulation of the members of the struct invalidates the matrix
        DirectX::XMFLOAT4X4 Matrix;
    };

    // Some useful helpers (REVIEW: should we move these elsewhere?)
    GDK_INLINE DirectX::XMFLOAT3    NegateFloat3    (_In_ const DirectX::XMFLOAT3& vector)      { return DirectX::XMFLOAT3(-vector.x, -vector.y, -vector.z); }
    GDK_INLINE DirectX::XMFLOAT4    NegateFloat4    (_In_ const DirectX::XMFLOAT4& vector)      { return DirectX::XMFLOAT4(-vector.x, -vector.y, -vector.z, -vector.w); }

    GDK_INLINE float                LengthFloat3    (_In_ const DirectX::XMFLOAT3& vector)      { return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); }
    GDK_INLINE DirectX::XMFLOAT3    NormalizeFloat3 (_In_ const DirectX::XMFLOAT3& vector)
    {
        float oneOverLength = 1.0f / LengthFloat3(vector);
        return DirectX::XMFLOAT3(vector.x * oneOverLength, vector.y * oneOverLength, vector.z * oneOverLength);
    }

    GDK_INLINE DirectX::XMFLOAT3    CrossFloat3     (_In_ const DirectX::XMFLOAT3& v1, _In_ const DirectX::XMFLOAT3& v2) { return DirectX::XMFLOAT3(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v2.z * v1.x, v1.x * v2.y - v2.x * v1.y); }

    // NOTE: As a convenience, many of the methods have two flavors. One for manipulation of XMFLOAT based parameters and one for XMVECTOR based parameters.

    // Getting basis vectors
    GDK_INLINE DirectX::XMFLOAT3    GetRight        (_In_ const DirectX::XMFLOAT4X4& matrix )   { return matrix.m[0]; }
    GDK_INLINE DirectX::XMFLOAT3    GetLeft         (_In_ const DirectX::XMFLOAT4X4& matrix )   { return NegateFloat3(matrix.m[0]); }
    GDK_INLINE DirectX::XMFLOAT3    GetUp           (_In_ const DirectX::XMFLOAT4X4& matrix )   { return matrix.m[1]; }
    GDK_INLINE DirectX::XMFLOAT3    GetDown         (_In_ const DirectX::XMFLOAT4X4& matrix )   { return NegateFloat3(matrix.m[1]); }
    GDK_INLINE DirectX::XMFLOAT3    GetForward      (_In_ const DirectX::XMFLOAT4X4& matrix )   { return matrix.m[2]; }
    GDK_INLINE DirectX::XMFLOAT3    GetBackward     (_In_ const DirectX::XMFLOAT4X4& matrix )   { return NegateFloat3(matrix.m[2]); }

    GDK_INLINE DirectX::XMVECTOR    GetRight        (_In_ const DirectX::XMMATRIX& matrix )     { return matrix.r[0]; }
    GDK_INLINE DirectX::XMVECTOR    GetLeft         (_In_ const DirectX::XMMATRIX& matrix )     { return DirectX::XMVectorNegate(matrix.r[0]); }
    GDK_INLINE DirectX::XMVECTOR    GetUp           (_In_ const DirectX::XMMATRIX& matrix )     { return matrix.r[1]; }
    GDK_INLINE DirectX::XMVECTOR    GetDown         (_In_ const DirectX::XMMATRIX& matrix )     { return DirectX::XMVectorNegate(matrix.r[1]); }
    GDK_INLINE DirectX::XMVECTOR    GetForward      (_In_ const DirectX::XMMATRIX& matrix )     { return matrix.r[2]; }
    GDK_INLINE DirectX::XMVECTOR    GetBackward     (_In_ const DirectX::XMMATRIX& matrix )     { return DirectX::XMVectorNegate(matrix.r[2]); }

    // Building an affine transformation from components
    GDK_INLINE DirectX::XMMATRIX    BuildMatrix     (_In_ const DirectX::XMFLOAT3& scale, _In_ const DirectX::XMFLOAT4& orientation, _In_ const DirectX::XMFLOAT3& translation)
    {
        DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));
        DirectX::XMVECTOR vscale = DirectX::XMLoadFloat3(&scale);
        mat.r[0] = DirectX::XMVectorScale(mat.r[0], DirectX::XMVectorGetX(vscale));
        mat.r[1] = DirectX::XMVectorScale(mat.r[1], DirectX::XMVectorGetY(vscale));
        mat.r[2] = DirectX::XMVectorScale(mat.r[2], DirectX::XMVectorGetZ(vscale));
        mat.r[3] = DirectX::XMVectorSet(translation.x, translation.y, translation.z, 1.0f);
        return mat;
    }

    GDK_INLINE DirectX::XMMATRIX    BuildMatrix (_In_ const DirectX::XMVECTOR& scale, _In_ const DirectX::XMVECTOR& orientation, _In_ const DirectX::XMVECTOR& translation)
    {
        DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orientation);
        mat.r[0] = DirectX::XMVectorScale(mat.r[0], DirectX::XMVectorGetX(scale));
        mat.r[1] = DirectX::XMVectorScale(mat.r[1], DirectX::XMVectorGetY(scale));
        mat.r[2] = DirectX::XMVectorScale(mat.r[2], DirectX::XMVectorGetZ(scale));
        mat.r[3] = DirectX::XMVectorSetW(translation, 1.0f);
        return mat;
    }

    // Extract components of an affine transformation
    GDK_INLINE DirectX::XMFLOAT3    GetTranslation  (_In_ const DirectX::XMFLOAT4X4& transform ){ return transform.m[3]; }
    GDK_INLINE DirectX::XMFLOAT3    GetScale        (_In_ const DirectX::XMFLOAT4X4& transform ){ return DirectX::XMFLOAT3(LengthFloat3(transform.m[0]), LengthFloat3(transform.m[1]), LengthFloat3(transform.m[2])); }
    GDK_INLINE DirectX::XMFLOAT4X4  GetRotation     (_In_ const DirectX::XMFLOAT4X4& transform )
    {
        DirectX::XMFLOAT3 row0 = NormalizeFloat3(transform.m[0]);
        DirectX::XMFLOAT3 row1 = NormalizeFloat3(transform.m[1]);
        DirectX::XMFLOAT3 row2 = NormalizeFloat3(transform.m[2]);
        return DirectX::XMFLOAT4X4( row0.x, row0.y, row0.z, 0.0f,
                                    row1.x, row1.y, row1.z, 0.0f,
                                    row2.x, row2.y, row2.z, 0.0f,
                                    0.0f,   0.0f,   0.0f,   0.0f);
    }

    GDK_INLINE DirectX::XMVECTOR    GetTranslation  (_In_ const DirectX::XMMATRIX& transform )  { return transform.r[3]; }
    GDK_INLINE DirectX::XMVECTOR    GetScale        (_In_ const DirectX::XMMATRIX& transform )  { return DirectX::XMVectorSet(DirectX::XMVectorGetX(DirectX::XMVector3Length(transform.r[0])), DirectX::XMVectorGetX(DirectX::XMVector3Length(transform.r[1])), DirectX::XMVectorGetX(DirectX::XMVector3Length(transform.r[2])), 0); }
    GDK_INLINE DirectX::XMMATRIX    GetRotation     (_In_ const DirectX::XMMATRIX& transform )
    {
        DirectX::XMMATRIX rotation;
        rotation.r[0] = DirectX::XMVector3Normalize(transform.r[0]);
        rotation.r[1] = DirectX::XMVector3Normalize(transform.r[1]);
        rotation.r[2] = DirectX::XMVector3Normalize(transform.r[2]);
        rotation.r[3] = DirectX::XMVectorZero();
        return rotation;
    }

    // View matrix
    GDK_INLINE DirectX::XMVECTOR    GetEyePoint     (_In_ const DirectX::XMFLOAT4X4& viewMatrix )
    {
        DirectX::XMMATRIX rotation = DirectX::XMLoadFloat4x4(&viewMatrix);
        DirectX::XMVECTOR translation = rotation.r[3];
        rotation.r[3] = DirectX::XMVectorZero();
        return DirectX::XMVectorNegate(DirectX::XMVector3Transform(translation, DirectX::XMMatrixTranspose(rotation)));
    }

    // orients the tranform such that it's forward vector points directly towards the target point
    GDK_INLINE void                 LookAtPoint     ( _Inout_ Transform& transform, _In_ const DirectX::XMVECTOR& targetPoint, _In_ bool rebuildMatrix = true )
    {
        if (rebuildMatrix)
            DirectX::XMStoreFloat4x4(&transform.Matrix, BuildMatrix(transform.Scale, transform.Orientation, transform.Position));

        DirectX::XMMATRIX rotation;

        rotation.r[2] = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetPoint, DirectX::XMLoadFloat3(&transform.Position)));
        // orthonormalize the right & up vector based on new forward
        DirectX::XMFLOAT3 up = GetUp(transform.Matrix);
        rotation.r[0] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), rotation.r[2]));
        rotation.r[1] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rotation.r[2], rotation.r[0]));

        DirectX::XMStoreFloat4(&transform.Orientation, DirectX::XMQuaternionRotationMatrix(rotation));
    }

    // aligns the up vector with some user defined vector
    GDK_INLINE void                 AlignUpVector   ( _Inout_ Transform& transform, _In_ const DirectX::XMVECTOR& targetUp, _In_ bool rebuildMatrix = true )
    {
        if (rebuildMatrix)
            DirectX::XMStoreFloat4x4(&transform.Matrix, BuildMatrix(transform.Scale, transform.Orientation, transform.Position));

        DirectX::XMMATRIX rotation;

        DirectX::XMFLOAT3 right = GetRight(transform.Matrix);
        rotation.r[0] = DirectX::XMVector3Cross(DirectX::XMVector3Cross(targetUp, DirectX::XMLoadFloat3(&right)), targetUp);
        // orthonormalize the right & up vector based on new forward
        DirectX::XMFLOAT3 forward = GetForward(transform.Matrix);
        rotation.r[1] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&forward), rotation.r[0]));
        rotation.r[2] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rotation.r[0], rotation.r[1]));

        DirectX::XMStoreFloat4(&transform.Orientation, DirectX::XMQuaternionRotationMatrix(rotation));
    }

    GDK_INLINE void                 Rotate          ( _Inout_ Transform& transform, _In_ const DirectX::XMVECTOR& axis, _In_ float angle )
    {
        DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(axis, angle);
        DirectX::XMStoreFloat4(&transform.Orientation, DirectX::XMQuaternionNormalize(DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.Orientation), quat)));
    }
}

