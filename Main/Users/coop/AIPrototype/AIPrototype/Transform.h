#pragma once

//==============================================================================
// Defines position, orientation, and scale. Also contains many utilities.
//
// Position - vector containing x, y, and z translation components. Measured in world units.
// Orientation - unit quaternion representing rotation.
// Scale - vector containing x, y, and z scale components. Non-uniform scale allowed.
//
//==============================================================================
class Transform
{
public:
    //
    // Creation
    //

    // Construct an identity transform
    Transform();

    // Copy from an existing one
    Transform(_In_ const Transform& other);

    // Move from an existing one
    Transform(_Inout_ Transform&& other);

    // Create from position, orientation, and scale
    Transform(_In_ const XMFLOAT3& position, _In_ const XMFLOAT4& orientation, _In_ const XMFLOAT3& scale);
    Transform(_In_ const XMVECTOR& position, _In_ const XMVECTOR& orientation, _In_ const XMVECTOR& scale);

    //
    // Assignment
    //

    Transform& operator= (_In_ const Transform& other);
    Transform& operator= (_Inout_ Transform&& other);

    //
    // Access components of the transform
    //

    const XMFLOAT3& GetPosition() const;
    const XMFLOAT4& GetOrientation() const;
    const XMFLOAT3& GetScale() const;
    const XMFLOAT4X4& GetWorld() const;

    XMFLOAT3 GetRight() const;
    XMFLOAT3 GetUp() const;
    XMFLOAT3 GetForward() const;

    // Return directly as vector register types for use in computation
    XMVECTOR GetPositionV() const;
    XMVECTOR GetOrientationV() const;
    XMVECTOR GetScaleV() const;
    XMMATRIX GetWorldV() const;

    XMVECTOR GetRightV() const;
    XMVECTOR GetUpV() const;
    XMVECTOR GetForwardV() const;

    //
    // Set components
    //

    void SetPosition(_In_ const XMFLOAT3& value);
    void SetPosition(_In_ const XMVECTOR& value);
    void SetOrientation(_In_ const XMFLOAT4& value);
    void SetOrientation(_In_ const XMVECTOR& value);
    void SetScale(_In_ const XMFLOAT3& value);
    void SetScale(_In_ const XMVECTOR& value);

    //
    // Utility methods
    //

    void LookAt(_In_ const XMFLOAT3& target, _In_ const XMFLOAT3& up);
    void LookAt(_In_ const XMVECTOR& target, _In_ const XMVECTOR& up);

private:
    XMFLOAT3 _position;
    XMFLOAT4 _orientation;
    XMFLOAT3 _scale;

    mutable bool _worldIsDirty;
    mutable XMFLOAT4X4 _world;
};
