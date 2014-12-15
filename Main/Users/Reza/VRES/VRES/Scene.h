#pragma once

namespace VRES
{
    class Scene;
    class Light;

    enum class SceneNodeType
    {
        Model = 0,
        Light,
    };

    //
    // All objects that can be placed into the scene
    // need to derive from SceneNode
    //
    class SceneNode : NonCopyable<SceneNode>, public std::enable_shared_from_this<SceneNode>
    {
    public:
        virtual ~SceneNode();

        SceneNodeType Type() const { return _type; }

        const VRES::Transform& Transform() const;
        VRES::Transform& Transform();

        const VRES::BoundingBox& BoundingBox() const;
        VRES::BoundingBox& BoundingBox();

    protected:
        SceneNode(SceneNodeType type);

        Scene* Scene() { return _scene; }

    private:
        friend class Scene;

        VRES::Scene* _scene;
        SceneNodeType _type;
        VRES::Transform _transform;
        VRES::BoundingBox _bounds;
    };

    class Scene : NonCopyable<Scene>, public std::enable_shared_from_this<Scene>
    {
    public:
        static std::shared_ptr<Scene> Create();
        ~Scene();

        void Insert(const std::shared_ptr<SceneNode>& node);
        void Remove(const std::shared_ptr<SceneNode>& node);

        uint32_t NumModels() const { return (uint32_t)_models.size(); }
        const Model* const* Models() const { return _models.data(); }

        uint32_t NumLights() const { return (uint32_t)_lights.size(); }
        const Light* const* Lights() const { return _lights.data(); }

    private:
        Scene();

    private:
        std::vector<std::shared_ptr<SceneNode>> _allNodes;
        std::vector<Model*> _models;
        std::vector<Light*> _lights;
    };
}
