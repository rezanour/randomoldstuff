#include <Precomp.h>
#include "ModelImpl.h"
#include "LightImpl.h"
#include "GraphicsSceneImpl.h"

std::shared_ptr<GraphicsScene> GraphicsScene::Create()
{
    return std::shared_ptr<GraphicsScene>(new GraphicsScene);
}

GraphicsScene::GraphicsScene()
{
}

void GraphicsScene::Clear()
{
    _opaqueModels.clear();
    _transparentModels.clear();
    _directionalLights.clear();
    _pointLights.clear();
}

_Use_decl_annotations_
void GraphicsScene::AddModel(const std::shared_ptr<IModel>& model)
{
    auto m = static_cast<Model*>(model.get())->shared_from_this();

    // Do we already know about this model?
    auto it = _opaqueModels.find(m->GetId());
    if (it != std::end(_opaqueModels))
    {
        return;
    }

    it = _transparentModels.find(m->GetId());
    if (it != std::end(_transparentModels))
    {
        return;
    }

    // New model for this scene, determine which list to put it in
    // and notify it of its new parent scene
    if (m->IsTransparent())
    {
        _transparentModels.insert(std::make_pair(m->GetId(), m));
    }
    else
    {
        _opaqueModels.insert(std::make_pair(m->GetId(), m));
    }

    m->SetScene(shared_from_this());
}

_Use_decl_annotations_
void GraphicsScene::RemoveModel(const std::shared_ptr<IModel>& model)
{
    auto m = static_cast<Model*>(model.get())->shared_from_this();

    _opaqueModels.erase(m->GetId());
    _transparentModels.erase(m->GetId());
    m->SetScene(nullptr);
}

_Use_decl_annotations_
void GraphicsScene::AddLight(const std::shared_ptr<ILight>& light)
{
    auto l = static_cast<Light*>(light.get())->shared_from_this();

    auto& lights = light->GetType() == LightType::Ambient ? _ambientLights :
                    (light->GetType() == LightType::Directional ? _directionalLights : _pointLights);

    auto it = std::find(std::begin(lights), std::end(lights), l);
    if (it == std::end(lights))
    {
        lights.push_back(l);
    }
}

_Use_decl_annotations_
void GraphicsScene::RemoveLight(const std::shared_ptr<ILight>& light)
{
    auto l = static_cast<Light*>(light.get())->shared_from_this();

    auto& lights = light->GetType() == LightType::Ambient ? _ambientLights :
        (light->GetType() == LightType::Directional ? _directionalLights : _pointLights);

    auto it = std::find(std::begin(lights), std::end(lights), l);
    if (it != std::end(lights))
    {
        lights.erase(it);
    }
}

std::shared_ptr<IDebugGraphicsScene> GraphicsScene::GetDebug()
{
    return shared_from_this();
}

_Use_decl_annotations_
void GraphicsScene::DrawLine(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT4& color)
{
    _debugLines.push_back(
        DebugLine
        {
            Position3DColorVertex{ v0, color },
            Position3DColorVertex{ v1, color }
        });
}

_Use_decl_annotations_
void GraphicsScene::DrawTriangle(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT4& color)
{
    ProfileMark profile(CodeTag::DebugRendering);

    _debugTriangles.push_back(
        DebugTriangle
        {
            Position3DColorVertex{ v0, color },
            Position3DColorVertex{ v1, color },
            Position3DColorVertex{ v2, color }
        });
}

_Use_decl_annotations_
void GraphicsScene::DrawQuad(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, const XMFLOAT4& color)
{
    DrawTriangle(v0, v1, v2, color);
    DrawTriangle(v0, v2, v3, color);
}

_Use_decl_annotations_
void GraphicsScene::DrawBox(const XMFLOAT3& mins, const XMFLOAT3& maxs, const XMFLOAT4& color)
{
    DrawQuad(XMFLOAT3(mins.x, mins.y, mins.z), XMFLOAT3(mins.x, maxs.y, mins.z), XMFLOAT3(maxs.x, maxs.y, mins.z), XMFLOAT3(maxs.x, mins.y, mins.z), color);
    DrawQuad(XMFLOAT3(maxs.x, mins.y, mins.z), XMFLOAT3(maxs.x, maxs.y, mins.z), XMFLOAT3(maxs.x, maxs.y, maxs.z), XMFLOAT3(maxs.x, mins.y, maxs.z), color);
    DrawQuad(XMFLOAT3(maxs.x, mins.y, maxs.z), XMFLOAT3(maxs.x, maxs.y, maxs.z), XMFLOAT3(mins.x, maxs.y, maxs.z), XMFLOAT3(mins.x, mins.y, maxs.z), color);
    DrawQuad(XMFLOAT3(mins.x, mins.y, maxs.z), XMFLOAT3(mins.x, maxs.y, maxs.z), XMFLOAT3(mins.x, maxs.y, mins.z), XMFLOAT3(mins.x, mins.y, mins.z), color);
    DrawQuad(XMFLOAT3(mins.x, maxs.y, mins.z), XMFLOAT3(mins.x, maxs.y, maxs.z), XMFLOAT3(maxs.x, maxs.y, maxs.z), XMFLOAT3(maxs.x, maxs.y, mins.z), color);
    DrawQuad(XMFLOAT3(mins.x, mins.y, maxs.z), XMFLOAT3(mins.x, mins.y, mins.z), XMFLOAT3(maxs.x, mins.y, mins.z), XMFLOAT3(maxs.x, mins.y, maxs.z), color);
}

_Use_decl_annotations_
void GraphicsScene::GetAllOpaqueModels(std::vector<std::shared_ptr<Model>>& models)
{
    models.clear();
    for (auto& pair : _opaqueModels)
    {
        models.push_back(pair.second);
    }
}

_Use_decl_annotations_
void GraphicsScene::GetAllTransparentModels(std::vector<std::shared_ptr<Model>>& models)
{
    models.clear();
    for (auto& pair : _transparentModels)
    {
        models.push_back(pair.second);
    }
}

_Use_decl_annotations_
void GraphicsScene::GetAllAmbientLights(std::vector<std::shared_ptr<Light>>& lights)
{
    lights.clear();
    lights.insert(std::begin(lights), std::begin(_ambientLights), std::end(_ambientLights));
}

_Use_decl_annotations_
void GraphicsScene::GetAllDirectionalLights(std::vector<std::shared_ptr<Light>>& lights)
{
    lights.clear();
    lights.insert(std::begin(lights), std::begin(_directionalLights), std::end(_directionalLights));
}

_Use_decl_annotations_
void GraphicsScene::GetAllPointLights(std::vector<std::shared_ptr<Light>>& lights)
{
    lights.clear();
    lights.insert(std::begin(lights), std::begin(_pointLights), std::end(_pointLights));
}

_Use_decl_annotations_
void GraphicsScene::ModelChangedTransparency(const std::shared_ptr<Model>& model, bool newTransparency)
{
    if (newTransparency)
    {
        _opaqueModels.erase(model->GetId());
        _transparentModels.insert(std::make_pair(model->GetId(), model));
    }
    else
    {
        _transparentModels.erase(model->GetId());
        _opaqueModels.insert(std::make_pair(model->GetId(), model));
    }
}

bool GraphicsScene::HasDebugContent() const
{
    return (_debugLines.size() > 0) || (_debugTriangles.size() > 0);
}

void GraphicsScene::ClearDebugContent()
{
    _debugLines.clear();
    _debugTriangles.clear();
}

_Use_decl_annotations_
void GraphicsScene::GetDebugTriangles(std::vector<DebugTriangle>& triangles)
{
    triangles.clear();
    triangles.insert(std::begin(triangles), std::begin(_debugTriangles), std::end(_debugTriangles));
}

_Use_decl_annotations_
void GraphicsScene::GetDebugLines(std::vector<DebugLine>& lines)
{
    lines.clear();
    lines.insert(std::begin(lines), std::begin(_debugLines), std::end(_debugLines));
}

