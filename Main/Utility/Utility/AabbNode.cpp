#include <Utility.h>
#include <memory>
#include <assert.h>

static AabbNode* AabbNodeNew();
static void AabbNodeDelete(_In_ AabbNode* node);

static pfnAabbNodeAlloc g_pfnAlloc = AabbNodeNew;
static pfnAabbNodeFree g_pfnFree = AabbNodeDelete;
static pfnAabbNodeGetBoundsFromObject g_pfnGetBounds;

_Use_decl_annotations_
void AabbNodeStartup(pfnAabbNodeGetBoundsFromObject pfnGetBounds)
{
    assert(pfnGetBounds != nullptr);
    g_pfnGetBounds = pfnGetBounds;
}

_Use_decl_annotations_
void AabbNodeSetAllocFree(pfnAabbNodeAlloc pfnAlloc, pfnAabbNodeFree pfnFree)
{
    assert(pfnAlloc != nullptr && pfnFree != nullptr);

    g_pfnAlloc = pfnAlloc;
    g_pfnFree = pfnFree;
}

_Use_decl_annotations_
bool AabbNodeIsLeaf(const AabbNode* node)
{
    return (node->Right == nullptr);
}

_Use_decl_annotations_
void AabbNodeMarkDirty(AabbNode* node)
{
    node->BoundsDirty = true;
    if (node->Parent != nullptr)
    {
        AabbNodeMarkDirty(node->Parent);
    }
}

_Use_decl_annotations_
void AabbNodeGetBounds(AabbNode* node, float mins[3], float maxs[3])
{
    if (node->BoundsDirty)
    {
        if (AabbNodeIsLeaf(node))
        {
            g_pfnGetBounds(node->Object, node->Mins, node->Maxs);
        }
        else
        {
            float mins1[3], maxs1[3];
            float mins2[3], maxs2[3];
            AabbNodeGetBounds(node->Left, mins1, maxs1);
            AabbNodeGetBounds(node->Right, mins2, maxs2);
            node->Mins[0] = fminf(mins1[0], mins2[0]);
            node->Mins[1] = fminf(mins1[1], mins2[1]);
            node->Mins[2] = fminf(mins1[2], mins2[2]);
            node->Maxs[0] = fmaxf(maxs1[0], maxs2[0]);
            node->Maxs[1] = fmaxf(maxs1[1], maxs2[1]);
            node->Maxs[2] = fmaxf(maxs1[2], maxs2[2]);
        }

        node->BoundsDirty = false;
    }

    memcpy(mins, node->Mins, sizeof(node->Mins));
    memcpy(maxs, node->Maxs, sizeof(node->Maxs));
}

_Use_decl_annotations_
AabbNode* AabbNodeInsert(AabbNode** root, const float mins[3], const float maxs[3], void* object)
{
    assert(root != nullptr);

    if (*root == nullptr)
    {
        // create a new leaf
        AabbNode* node = g_pfnAlloc();
        node->Object = object;
        node->Parent = nullptr;
        node->Right = nullptr;
        memcpy(node->Mins, mins, sizeof(node->Mins));
        memcpy(node->Maxs, maxs, sizeof(node->Maxs));
        node->BoundsDirty = false;
        *root = node;
        return node;
    }
    else
    {
        if (AabbNodeIsLeaf(*root))
        {
            // insert a new inner node here, and make current *root + new node as children
            // create the inner node
            AabbNode* innerNode = g_pfnAlloc();
            innerNode->Parent = (*root)->Parent;
            (*root)->Parent = innerNode;
            AabbNodeMarkDirty(innerNode);

            // create a new leaf
            AabbNode* node = g_pfnAlloc();
            node->Object = object;
            node->Parent = innerNode;
            node->Right = nullptr;
            memcpy(node->Mins, mins, sizeof(node->Mins));
            memcpy(node->Maxs, maxs, sizeof(node->Maxs));
            node->BoundsDirty = false;

            innerNode->Left = *root;
            innerNode->Right = node;

            *root = innerNode;

            return node;
        }
        else
        {
            // TODO: pick which child is the better fit to descend
            return AabbNodeInsert(&((*root)->Left), mins, maxs, object);
        }
    }
}

_Use_decl_annotations_
void AabbNodeRemove(AabbNode** root, AabbNode* node)
{
    assert(root != nullptr);

    if (node->Parent != nullptr)
    {
        AabbNode* grandParent = nullptr;
        grandParent = node->Parent->Parent;

        AabbNode* other = nullptr;
        if (node->Parent->Left == node)
        {
            other = node->Parent->Right;
        }
        else
        {
            other = node->Parent->Left;
        }

        if (other != nullptr)
        {
            other->Parent = grandParent;
        }

        if (grandParent != nullptr)
        {
            if (grandParent->Left == node->Parent)
            {
                grandParent->Left = other;
            }
            else
            {
                grandParent->Right = other;
            }
        }
        else
        {
            assert(*root == node->Parent);
            *root = other;
        }

        node->Parent->Left = nullptr;
        node->Parent->Right = nullptr;
        g_pfnFree(node->Parent);
    }

    if (AabbNodeIsLeaf(node))
    {
        g_pfnFree(node);
    }
    else
    {
        AabbNode* right = node->Right;
        AabbNodeRemove(root, node->Left);
        AabbNodeRemove(root, right);
    }

    if (node == (*root))
    {
        *root = nullptr;
    }
}

AabbNode* AabbNodeNew()
{
    assert(g_pfnGetBounds != nullptr);
    return new AabbNode;
}

_Use_decl_annotations_
void AabbNodeDelete(AabbNode* node)
{
    delete node;
}

