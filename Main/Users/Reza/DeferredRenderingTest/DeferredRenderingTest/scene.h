#pragma once

class Object;
class Light;

class Scene
{
public:
    void AddObject(const std::shared_ptr<Object>& object);
    void AddLight(const std::shared_ptr<Light>& light);

    // Object queries
    void GetObjectsInSphere(const XMFLOAT3& position, float radius, std::vector<std::shared_ptr<Object>>& objects);

    // Light queries
    void GetLightsInSphere(const XMFLOAT3& position, float radius, std::vector<std::shared_ptr<Light>>& lights);

private:
    std::vector<std::shared_ptr<Object>> _objects;
    std::vector<std::shared_ptr<Light>> _lights;
};
