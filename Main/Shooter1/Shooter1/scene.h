#pragma once

class Scene;
class GameObject;

class SceneNode : public TrackedObject<MemoryTag::SceneNode>
{
public:
    ~SceneNode();

    bool IsLeaf() const;
    bool IsInner() const;

    const AABB& GetBounds();
    void MarkBoundsDirty();

    // Parents, by definition, can only be inner nodes
    SceneNode* GetParent() const;
    SceneNode* GetLeft() const;
    SceneNode* GetRight() const;
    GameObject* GetObject() const;

    SceneNode* GetOtherChild(_In_ SceneNode* existingChild);
    void ReplaceChild(_In_ SceneNode* existingChild, _In_opt_ SceneNode* replacement);

private:
    friend class Scene;
    SceneNode(_In_ GameObject* object);                     // creates leaf node
    SceneNode(_In_ SceneNode* left, _In_ SceneNode* right); // creates inner node
    SceneNode(const SceneNode&);

    SceneNode* _parent;
    union
    {
        SceneNode* _left;
        GameObject* _object;
    };
    SceneNode* _right;

    AABB _bounds;
    bool _boundsDirty : 1;
};

class Scene : public TrackedObject<MemoryTag::Scene>
{
public:
    Scene();
    ~Scene();

    void AddObject(_In_ GameObject* object);
    void RemoveObject(_In_ GameObject* object);

private:
    Scene(const Scene&);

    void AddObject(_In_ SceneNode* node, _In_ GameObject* object);

private:
    SceneNode* _root;
};
