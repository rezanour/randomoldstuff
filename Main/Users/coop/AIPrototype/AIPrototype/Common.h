#pragma once

//===============================================================
// Camera
//===============================================================

struct __declspec(novtable) ICamera
{
    virtual const Transform& GetTransform() const = 0;

    virtual const XMFLOAT4X4& GetView() const = 0;
    virtual const XMFLOAT4X4& GetProjection() const = 0;

    virtual XMMATRIX GetViewV() const = 0;
    virtual XMMATRIX GetProjectionV() const = 0;
};

//===============================================================
// Game Object
//===============================================================

struct __declspec(novtable) IGameObject
{
    virtual const Transform& GetTransform() const = 0;
};

//===============================================================
// Game Scene Query
//===============================================================

struct __declspec(novtable) ISceneQuery
{
    virtual void GetAllObjects(_Out_ std::vector<IGameObject>& objects) const = 0;
};

