#pragma once

#include <stde\types.h>
#include <L3DMath\L3DMath.h>
#include <vector>
#include <map>

#include "BaseShader.h"
#include "Geometry.h"
#include "Material.h"

namespace Lucid3D
{
    class LightComponent;

    namespace TaskType
    {
        enum Enum
        {
            RenderTask,
            LightTask,
        };
    };

    // RenderTask is a single 'bundle' to send through the graphics pipeline.
    // This could be a mesh to render during the geometry pass, or a quad representing
    // a light to render during the lighting pass, etc...
    struct RenderTask
    {
        TaskType::Enum Type;
        L3DMath::Matrix Matrix;
        GeometryPtr Geometry;
        MaterialPtr Material;

        RenderTask(_In_ const L3DMath::Matrix& matrix, _In_ GeometryPtr& geometry, _In_ MaterialPtr& material, _In_ ShaderPtr& shader)
            : Matrix(matrix), Geometry(geometry), Material(material)
        {
            Type = TaskType::RenderTask;
            // Ensure that the geometry has an input layout for this particular shader.
            // This method no-ops if there's already a matching layout
            geometry->GenerateInputLayout(shader);
        }
    };

    struct LightRenderTask : public RenderTask
    {
        LightComponent* Light;

        LightRenderTask(_In_ const L3DMath::Matrix& matrix, _In_ GeometryPtr& geometry, _In_ MaterialPtr& material, _In_ ShaderPtr& shader, _In_ LightComponent* light)
            : RenderTask(matrix, geometry, material, shader), Light(light)
        {
            Type = TaskType::LightTask;
        }
    };

    typedef std::shared_ptr<RenderTask> RenderTaskPtr;
    typedef std::map<ShaderPtr, std::vector<RenderTaskPtr>> RenderMap;
}

