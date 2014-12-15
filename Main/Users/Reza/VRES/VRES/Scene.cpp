#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

SceneNode::SceneNode(SceneNodeType type) :
    _type(type), _scene(nullptr)
{
}

SceneNode::~SceneNode()
{
    if (_scene != nullptr)
    {
        // remove from scene
        _scene->Remove(shared_from_this());
    }
}

const Transform& SceneNode::Transform() const
{
    return _transform;
}

Transform& SceneNode::Transform()
{
    return _transform;
}

const BoundingBox& SceneNode::BoundingBox() const
{
    return _bounds;
}

BoundingBox& SceneNode::BoundingBox()
{
    return _bounds;
}

std::shared_ptr<Scene> Scene::Create()
{
    return std::shared_ptr<Scene>(new Scene);
}

Scene::Scene()
{
}

Scene::~Scene()
{
    for (auto& node : _allNodes)
    {
        node->_scene = nullptr;
    }
}

void Scene::Insert(const std::shared_ptr<SceneNode>& node)
{
    if (node->_scene != nullptr)
    {
        assert(false);
        return;
    }

    switch (node->Type())
    {
    case SceneNodeType::Model:
        _models.push_back(static_cast<Model*>(node.get()));
        break;

    case SceneNodeType::Light:
        _lights.push_back(static_cast<Light*>(node.get()));
        break;

    default:
        assert(false);
        return;
    }

    node->_scene = this;
    _allNodes.push_back(node);
}

void Scene::Remove(const std::shared_ptr<SceneNode>& node)
{
    if (node->_scene != this)
    {
        assert(false);
        return;
    }

    switch (node->Type())
    {
    case SceneNodeType::Model:
    {
        auto it = std::find(_models.begin(), _models.end(), static_cast<Model*>(node.get()));
        if (it != _models.end())
        {
            _models.erase(it);
        }
        break;
    }

    case SceneNodeType::Light:
    {
        auto it = std::find(_lights.begin(), _lights.end(), static_cast<Light*>(node.get()));
        if (it != _lights.end())
        {
            _lights.erase(it);
        }
        break;
    }

    default:
        assert(false);
        return;
    }

    node->_scene = nullptr;
    auto it = std::find(_allNodes.begin(), _allNodes.end(), node);
    if (it != _allNodes.end())
    {
        _allNodes.erase(it);
    }
}
