#pragma once

#include <GraphicsScene.h>
#include <Geometry.h>
#include <Model.h>
#include <Light.h>

class Model;
class Light;

// Structs to bundle up a primitive, which aids in
// sorting them
struct DebugLine
{
    Position3DColorVertex v0, v1;
};

struct DebugTriangle
{
    Position3DColorVertex v0, v1, v2;
};

class GraphicsScene : public BaseObject<GraphicsScene>, public IGraphicsScene, public IDebugGraphicsScene
{
public:
    static std::shared_ptr<GraphicsScene> Create();

    //
    // IGraphicsScene
    //

    void Clear() override;
    void AddModel(_In_ const std::shared_ptr<IModel>& model) override;
    void RemoveModel(_In_ const std::shared_ptr<IModel>& model) override;
    void AddLight(_In_ const std::shared_ptr<ILight>& light) override;
    void RemoveLight(_In_ const std::shared_ptr<ILight>& light) override;

    std::shared_ptr<IDebugGraphicsScene> GetDebug() override;

    //
    // IDebugGraphicsScene
    //

    void DrawLine(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT4& color) override;
    void DrawTriangle(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT3& v2, _In_ const XMFLOAT4& color) override;
    void DrawQuad(_In_ const XMFLOAT3& v0, _In_ const XMFLOAT3& v1, _In_ const XMFLOAT3& v2, _In_ const XMFLOAT3& v3, _In_ const XMFLOAT4& color) override;
    void DrawBox(_In_ const XMFLOAT3& mins, _In_ const XMFLOAT3& maxs, _In_ const XMFLOAT4& color) override;

    //
    // Internal
    //

    void GetAllOpaqueModels(_Out_ std::vector<std::shared_ptr<Model>>& models);
    void GetAllTransparentModels(_Out_ std::vector<std::shared_ptr<Model>>& models);
    void GetAllAmbientLights(_Out_ std::vector<std::shared_ptr<Light>>& lights);
    void GetAllDirectionalLights(_Out_ std::vector<std::shared_ptr<Light>>& lights);
    void GetAllPointLights(_Out_ std::vector<std::shared_ptr<Light>>& lights);

    void ModelChangedTransparency(_In_ const std::shared_ptr<Model>& model, _In_ bool newTransparency);

    // Debug support
    bool HasDebugContent() const;
    void ClearDebugContent();
    void GetDebugTriangles(_Out_ std::vector<DebugTriangle>& triangles);
    void GetDebugLines(_Out_ std::vector<DebugLine>& lines);

private:
    GraphicsScene();

private:
    // TODO: implement something better once this is inadequate for performance
    std::hash_map<uint32_t, std::shared_ptr<Model>> _opaqueModels;
    std::hash_map<uint32_t, std::shared_ptr<Model>> _transparentModels;
    std::vector<std::shared_ptr<Light>> _ambientLights;
    std::vector<std::shared_ptr<Light>> _directionalLights;
    std::vector<std::shared_ptr<Light>> _pointLights;

    std::vector<DebugTriangle> _debugTriangles;
    std::vector<DebugLine> _debugLines;
};
