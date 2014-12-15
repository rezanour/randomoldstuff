#pragma once

struct IModel;
struct ILight;
struct IDebugGraphicsScene;

struct __declspec(novtable) IGraphicsScene
{
    virtual void Clear() = 0;

    virtual void AddModel(_In_ const std::shared_ptr<IModel>& model) = 0;
    virtual void RemoveModel(_In_ const std::shared_ptr<IModel>& model) = 0;

    virtual void AddLight(_In_ const std::shared_ptr<ILight>& light) = 0;
    virtual void RemoveLight(_In_ const std::shared_ptr<ILight>& light) = 0;

    virtual std::shared_ptr<IDebugGraphicsScene> GetDebug() = 0;
};

struct __declspec(novtable) IDebugGraphicsScene
{
    virtual void DrawLine(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT4& color) = 0;
    virtual void DrawTriangle(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT3& v2, _In_ const XMFLOAT4& color) = 0;
    virtual void DrawQuad(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT3& v2, _In_ const XMFLOAT3& v3, _In_ const XMFLOAT4& color) = 0;
    virtual void DrawBox(_In_ const XMFLOAT3& mins, _In_ const XMFLOAT3& maxs, _In_ const XMFLOAT4& color) = 0;
};
