#include "common.h"
#include "mapp.h"


static int* g_subsectors;
static int g_maxSubsectors;
static int* g_numSubsectors;

static vector2_t g_position;

// for culling of bsp nodes during rendering.
// these are the normals for the left and right side of view frustum
static vector2_t g_leftNorm;
static vector2_t g_rightNorm;

//
// Private BSP method declarations
//

static int ProcessNode(node_t* node, BOOL buildList);
static int ProcessChild(ushort_t child, BOOL buildList);
static BOOL IsLeafNode(ushort_t child);
static ushort_t GetIndexFromChild(ushort_t child);
static BOOL IsInFront(node_t* node);
static BOOL IsInView(boundingbox_t box);

//
// Public BSP methods
//

int BSPFindSubsector(vector2_t position)
{
    node_t* root = &g_map.Nodes[g_map.NumNodes - 1];
    g_position = position;
    return ProcessNode(root, FALSE);
}

void BSPTraverseForRendering(vector2_t position, vector2_t fovLeft, vector2_t fovRight, int* subsectors, int maxSubsectors, int* numSubsectors)
{
    node_t* root = &g_map.Nodes[g_map.NumNodes - 1];

    // store pointers to simplify the signatures for these traversal methods
    g_subsectors = subsectors;
    g_maxSubsectors = maxSubsectors;
    g_numSubsectors = numSubsectors;

    *g_numSubsectors = 0;

    g_position = position;
    g_leftNorm = Vector2Scale(Vector2Perp(fovLeft), -1);
    g_rightNorm = Vector2Perp(fovRight);

    ProcessNode(root, TRUE);
}


//
// Private BSP methods
//

int ProcessNode(node_t* node, BOOL buildList)
{
    if (buildList && *g_numSubsectors >= g_maxSubsectors)
    {
        return -1;
    }

    if (IsInFront(node))
    {
        if (buildList)
        {
            if (IsInView(node->RightBBox))
            {
                ProcessChild(node->RightChild, buildList);
            }
            if (IsInView(node->LeftBBox))
            {
                ProcessChild(node->LeftChild, buildList);
            }
        }
        else
        {
            return ProcessChild(node->RightChild, buildList);
        }
    }
    else
    {
        if (buildList)
        {
            if (IsInView(node->LeftBBox))
            {
                ProcessChild(node->LeftChild, buildList);
            }
            if (IsInView(node->RightBBox))
            {
                ProcessChild(node->RightChild, buildList);
            }
        }
        else
        {
            return ProcessChild(node->LeftChild, buildList);
        }
    }

    return 0;
}

int ProcessChild(ushort_t child, BOOL buildList)
{
    if (IsLeafNode(child))
    {
        if (buildList)
        {
            g_subsectors[(*g_numSubsectors)++] = GetIndexFromChild(child);
        }

        return GetIndexFromChild(child);
    }
    else
    {
        return ProcessNode(&g_map.Nodes[GetIndexFromChild(child)], buildList);
    }
}

BOOL IsLeafNode(ushort_t child)
{
    return (child & 0x8000) != 0;
}

ushort_t GetIndexFromChild(ushort_t child)
{
    return child & 0x7FFF;
}

BOOL IsInFront(node_t* node)
{
    vector2_t norm = Vector2Perp(Vector2(node->xDir, node->yDir));
    vector2_t dir = Vector2Subtract(g_position, Vector2(node->xStart, node->yStart));
    return Vector2Dot(norm, dir) >= 0;
}

BOOL IsInView(boundingbox_t box)
{
#ifdef OLD
    box;
    return TRUE;
#else
    vector2_t a, b, c, d;

    a = Vector2Subtract(Vector2(box.left, box.top), g_position);
    b = Vector2Subtract(Vector2(box.right, box.top), g_position);
    c = Vector2Subtract(Vector2(box.left, box.bottom), g_position);
    d = Vector2Subtract(Vector2(box.right, box.bottom), g_position);

    if (Vector2Dot(a, g_leftNorm) > 0 &&
        Vector2Dot(b, g_leftNorm) > 0 &&
        Vector2Dot(c, g_leftNorm) > 0 &&
        Vector2Dot(d, g_leftNorm) > 0)
    {
        return FALSE;
    }

    if (Vector2Dot(a, g_rightNorm) > 0 &&
        Vector2Dot(b, g_rightNorm) > 0 &&
        Vector2Dot(c, g_rightNorm) > 0 &&
        Vector2Dot(d, g_rightNorm) > 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}
