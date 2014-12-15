#include "precomp.h"
#include "scene.h"

void Scene::AddObject(const std::shared_ptr<Object>& object)
{
    _objects.push_back(object);
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
    _lights.push_back(light);
}

void Scene::GetObjectsInSphere(const XMFLOAT3& position, float radius, std::vector<std::shared_ptr<Object>>& objects)
{
    UNREFERENCED_PARAMETER(position);
    UNREFERENCED_PARAMETER(radius);

    objects.clear();
    objects.insert(std::begin(objects), std::begin(_objects), std::end(_objects));
}

void Scene::GetLightsInSphere(const XMFLOAT3& position, float radius, std::vector<std::shared_ptr<Light>>& lights)
{
    UNREFERENCED_PARAMETER(position);
    UNREFERENCED_PARAMETER(radius);

    lights.clear();
    lights.insert(std::begin(lights), std::begin(_lights), std::end(_lights));
}
