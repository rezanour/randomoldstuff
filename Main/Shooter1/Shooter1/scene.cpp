#include "precomp.h"
#include "scene.h"
#include "gameobject.h"

//==============================================================================

_Use_decl_annotations_
SceneNode::SceneNode(GameObject* object) :
    _parent(nullptr),
    _object(object),
    _right(nullptr),
    _bounds(),
    _boundsDirty(true)
{
    _object->_node = this;
}

_Use_decl_annotations_
SceneNode::SceneNode(SceneNode* left, SceneNode* right) :
    _parent(nullptr),
    _left(left),
    _right(right),
    _bounds(),
    _boundsDirty(true)
{
    _left->_parent = this;
    _right->_parent = this;
}

SceneNode::~SceneNode()
{
    if (IsLeaf())
    {
        _object->_node = nullptr;
    }
    else
    {
        delete _left;
        delete _right;
    }

    if (_parent != nullptr)
    {
        _parent->ReplaceChild(this, nullptr);
    }
}

bool SceneNode::IsLeaf() const
{
    return (_right == nullptr);
}

bool SceneNode::IsInner() const
{
    return !IsLeaf();
}

const AABB& SceneNode::GetBounds()
{
    if (_boundsDirty)
    {
        if (IsLeaf())
        {
            _bounds = _object->GetBounds();
        }
        else
        {
            _bounds = _left->GetBounds();
            _bounds.Add(_right->GetBounds());
        }
        _boundsDirty = false;
    }

    return _bounds;
}

void SceneNode::MarkBoundsDirty()
{
    _boundsDirty = true;
    if (_parent != nullptr)
    {
        _parent->MarkBoundsDirty();
    }
}

SceneNode* SceneNode::GetParent() const
{
    return _parent;
}

SceneNode* SceneNode::GetLeft() const
{
    return _left;
}

SceneNode* SceneNode::GetRight() const
{
    return _right;
}

GameObject* SceneNode::GetObject() const
{
    return _object;
}

_Use_decl_annotations_
SceneNode* SceneNode::GetOtherChild(SceneNode* existingChild)
{
    if (_left == existingChild)
    {
        return _right;
    }
    else if (_right == existingChild)
    {
        return _left;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

_Use_decl_annotations_
void SceneNode::ReplaceChild(SceneNode* existingChild, SceneNode* replacement)
{
    if (_left == existingChild)
    {
        existingChild->_parent = nullptr;
        _left = replacement;
        if (replacement != nullptr)
        {
            replacement->_parent = this;
        }
    }
    else if (_right == existingChild)
    {
        existingChild->_parent = nullptr;
        _right = replacement;
        if (replacement != nullptr)
        {
            replacement->_parent = this;
        }
    }
    else
    {
        assert(false);
    }
}

//==============================================================================

Scene::Scene() :
    _root(nullptr)
{
}

Scene::~Scene()
{
    delete _root;
    _root = nullptr;
}

_Use_decl_annotations_
void Scene::AddObject(GameObject* object)
{
    assert(object->_node == nullptr);

    if (_root == nullptr)
    {
        _root = new SceneNode(object);
    }
    else
    {
        AddObject(_root, object);
    }
}

_Use_decl_annotations_
void Scene::RemoveObject(GameObject* object)
{
    SceneNode* node = object->_node;

    assert(node != nullptr);

    if (_root == node)
    {
        _root = nullptr;
    }
    else
    {
        SceneNode* parent = node->GetParent();
        assert(parent != nullptr);

        // delete our parent with us, promoting the other child
        SceneNode* otherChild = parent->GetOtherChild(node);
        parent->_left = nullptr;
        parent->_right = nullptr;

        SceneNode* grandParent = parent->GetParent();
        if (grandParent != nullptr)
        {
            grandParent->ReplaceChild(parent, otherChild);
            grandParent->MarkBoundsDirty();
        }
        else
        {
            assert(_root == parent);
            _root = otherChild;
        }

        delete parent;
    }

    delete node;
}

_Use_decl_annotations_
void Scene::AddObject(SceneNode* node, GameObject* object)
{
    if (node->IsInner())
    {
        // Find which child we should descend. We want to pick
        // the side for which we add the least new growth
        SceneNode* left = node->GetLeft();
        SceneNode* right = node->GetRight();

        AABB leftBounds = left->GetBounds();
        AABB rightBounds = right->GetBounds();

        leftBounds.Add(object->GetBounds());
        rightBounds.Add(object->GetBounds());

        float leftExpand = leftBounds.GetVolume() - left->GetBounds().GetVolume();
        float rightExpand = rightBounds.GetVolume() - right->GetBounds().GetVolume();

        AddObject(leftExpand < rightExpand ? left : right, object);
    }
    else
    {
        SceneNode* parent = node->GetParent();

        // Insert a new inner node and make the two leaves it's children
        SceneNode* inner = new SceneNode(node, new SceneNode(object));

        if (parent != nullptr)
        {
            parent->ReplaceChild(node, inner);
        }
        else
        {
            assert(_root == node);
            _root = inner;
        }

        inner->MarkBoundsDirty();
    }
}
