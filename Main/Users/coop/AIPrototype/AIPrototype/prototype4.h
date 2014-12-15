#pragma once

#include "GameObject.h"

class GameWorld;
class GameObject;
class Waypoint;
class IGameWorld
{
public:
    virtual std::shared_ptr<GameObject> FindPlayer() = 0;
    virtual std::shared_ptr<Waypoint> FindWaypoint(int id) = 0;
    virtual std::shared_ptr<Waypoint> FindClosestWaypoint(Vector2 pos) = 0;
};

class GameContext
{
public:
    GameContext() :
        graphics(nullptr),
        world(nullptr),
        elapsedTime(0.0f)
    {

    }
    Game* graphics;
    float elapsedTime;
    GameWorld* world;
    IGameWorld* iworld;
};

typedef enum ObjectType
{
    OT_None,
    OT_Wall,
    OT_Player,
    OT_LightSoldier,
    OT_Waypoint,
    OT_Door
};

class GameObject
{
public:
    GameObject() :
        position(0.0f),
        orientation(0.0f),
        id(0),
        isDead(false),
        type(OT_None),
        isDeleted(false),
        currentSpeed(0.0f),
        maxForwardSpeed(50.f),
        maxTurnSpeed(10.0f),
        targetObject(nullptr),
        boundingRadius(2)
    {
    };

    void Delete() { isDeleted = true;}

    virtual void Update(_In_ GameContext* context) = 0;

    Vector2 position;
    float orientation;
    BoundingSphere boundingSphere;
    int id;
    bool isDead;
    bool isDeleted;
    ObjectType type;
    Vector2 velocity;
    float currentSpeed;
    float maxForwardSpeed;
    float maxTurnSpeed;
    std::shared_ptr<GameObject> targetObject;
    float boundingRadius;
};

class Behavior
{
public:
    virtual void Update(_In_ GameContext* context, _In_ GameObject* object) = 0;
};

class UserInputBehavior : public Behavior
{
public:
    void Update(_In_ GameContext* context, _In_ GameObject* object)
    {
        // Handle input behavior
        float speed = (object->maxForwardSpeed * context->elapsedTime);
        float turnSpeed = (object->maxTurnSpeed * context->elapsedTime);
        bool movePlayer = (Keyboard::IsKeyHeld(VK_RIGHT) ||
                           Keyboard::IsKeyHeld(VK_LEFT) ||
                           Keyboard::IsKeyHeld(VK_UP) ||
                           Keyboard::IsKeyHeld(VK_DOWN));

        // Calculate current speed, heading and initialize possible seek position
        object->currentSpeed += movePlayer ? speed : 0;
        Vector2 heading = Vector2((float)cos(object->orientation), (float)sin(object->orientation));

        // If a move operation was detected, apply it now.
        if (movePlayer)
        {
            Vector2 seekPos = object->position;
            // Adjust seek position from user input
            if (Keyboard::IsKeyHeld(VK_DOWN))  { seekPos.y += 10; }
            if (Keyboard::IsKeyHeld(VK_LEFT))  { seekPos.x -= 10; }
            if (Keyboard::IsKeyHeld(VK_RIGHT)) { seekPos.x += 10; }
            if (Keyboard::IsKeyHeld(VK_UP))    { seekPos.y -= 10; }

            object->orientation = TurnToFace(object->position, seekPos, object->orientation, turnSpeed);
            heading = Vector2((float)cos(object->orientation), (float)sin(object->orientation));
        }

        // Calculate new position
        object->position += heading * object->currentSpeed;

        // slow down to full stop using the same speed as the acceleration speed.
        object->currentSpeed -= speed;
        if (object->currentSpeed < 0)
        {
            object->currentSpeed = 0;
        }
    }
};

class MoveToTargetPositionBehavior : public Behavior
{
public:
    void Update(_In_ GameContext* context, _In_ GameObject* object)
    {
        if (object->targetObject)
        {
            // Handle input behavior
            float speed = (object->maxForwardSpeed * context->elapsedTime);
            float turnSpeed = (object->maxTurnSpeed * context->elapsedTime);

            // Calculate current speed, heading and initialize possible seek position
            object->currentSpeed += speed;
            // If a move operation was detected, apply it now.
            Vector2 seekPos = object->targetObject->position;

            object->orientation = TurnToFace(object->position, seekPos, object->orientation, turnSpeed);
            Vector2 heading = Vector2((float)cos(object->orientation), (float)sin(object->orientation));

            // Calculate new position
            object->position += heading * object->currentSpeed;

            // slow down to full stop using the same speed as the acceleration speed.
            object->currentSpeed -= speed;
            if (object->currentSpeed < 0)
            {
                object->currentSpeed = 0;
            }
        }
    }
};

class Wall : public GameObject
{
public:
    Wall(Vector2 pt1, Vector2 pt2)
    {
        p1 = pt1;
        p2 = pt2;

        // Calculate wall normal
        Vector2 t = p2 - p1;
        t.Normalize();

        normal.x = -t.y;
        normal.y = t.x;
    }

    void Update(_In_ GameContext* context) {}

    Vector2 p1;
    Vector2 p2;
    Vector2 normal;
};

class Waypoint : public GameObject
{
public:
    Waypoint(Vector2 pt1, int nextWaypointId)
    {
        position = pt1;
        nextWaypoint = nextWaypointId;
    }

    void Update(_In_ GameContext* context) {}
    int nextWaypoint;
};

class Player : public GameObject
{
public:
    Player(_In_ Vector2 position, float orientation, int id)
    {
        this->position = position;
        this->orientation = orientation;
        this->id = id;
        type = OT_Player;
        maxForwardSpeed = 60.0f;
    };

    void Update(_In_ GameContext* context)
    {
        userInputBehavior.Update(context, this);
    }

    UserInputBehavior userInputBehavior;
};

class Soldier : public GameObject
{
public:
    Soldier(_In_ Vector2 position, float orientation, int id, ObjectType type)
    {
        this->position = position;
        this->orientation = orientation;
        this->id = id;
        this->type = type;
    };

    void Update(_In_ GameContext* context)
    {
        // Handle AI behavior
        if (targetObject == nullptr)
        {
            targetObject = context->iworld->FindClosestWaypoint(position);
            currentWaypoint = context->iworld->FindClosestWaypoint(position);
        }
        moveToTargetBehavior.Update(context, this);

        if (PointInCircle(position, targetObject->boundingRadius, targetObject->position))
        {
            targetObject = context->iworld->FindWaypoint(currentWaypoint->nextWaypoint);
            currentWaypoint = context->iworld->FindWaypoint(currentWaypoint->nextWaypoint);
        }
    }

    MoveToTargetPositionBehavior moveToTargetBehavior;
    std::shared_ptr<Waypoint> currentWaypoint;
};

class GameWorld : public IGameWorld
{
public:
    GameWorld() :
        objectId(1)
    {

    }

    std::shared_ptr<GameObject> FindPlayer()
    {
        for (size_t i = 0; i < dynamicObjects.size(); i++)
        {
            if (dynamicObjects[i]->type == OT_Player)
            {
                return dynamicObjects[i];
            }
        }
        return nullptr;
    }

    std::shared_ptr<Waypoint> FindWaypoint(int id)
    {
        return waypoints[id];
    }

    std::shared_ptr<Waypoint> FindClosestWaypoint(Vector2 pos)
    {
        int id = 0;
        float distance = 0.0f;
        for (size_t i = 0; i < waypoints.size(); i++)
        {
            if (distance == 0.0f)
            {
                distance = Vector2::Distance(waypoints[i]->position, pos);
            }
            else
            {
                if (Vector2::Distance(waypoints[i]->position, pos) < distance)
                {
                    id = i;
                    distance = Vector2::Distance(waypoints[i]->position, pos);
                }
            }
        }

        return waypoints[id];
    }

    bool LoadFromFile(_In_ GameContext* context, _In_ char* path)
    {
        dynamicObjects.clear();
        walls.clear();
        waypoints.clear();

        FILE* file = nullptr;
        char buf[80];
        char* line = nullptr;
        errno_t err = 0;
        err = fopen_s(&file, path, "r");
        if (err != 0)
        {
            return false;
        }

        do
        {
            ZeroMemory(buf, sizeof(buf));
            line = fgets(buf, _countof(buf), file);
            if (line && line[0] != 0 && line[0] != '\n' && line[0] != '/')
            {
                ObjectType objType;
                sscanf_s(buf, "%d", &objType);

                switch(objType)
                {
                case OT_LightSoldier:
                    {
                        Vector2 p1;
                        float orientation;
                        sscanf_s(buf, "%d %f %f %f", &objType, &p1.x, &p1.y, &orientation);
                        std::shared_ptr<GameObject> t = std::make_shared<Soldier>(
                        p1,
                        orientation,
                        GetNextObjectId(),
                        OT_LightSoldier);

                        dynamicObjects.push_back(t);
                    }
                    break;
                case OT_Wall:
                    {
                        Vector2 p1;
                        Vector2 p2;
                        sscanf_s(buf, "%d %f %f %f %f", &objType, &p1.x, &p1.y, &p2.x, &p2.y);
                        std::shared_ptr<Wall> w = std::make_shared<Wall>(p1, p2);
                        w->id = GetNextObjectId();

                        walls.push_back(w);
                    }
                    break;
                case OT_Player:
                    {
                        Vector2 p1;
                        float orientation;
                        sscanf_s(buf, "%d %f %f %f", &objType, &p1.x, &p1.y, &orientation);
                        std::shared_ptr<GameObject> t = std::make_shared<Player>(
                            p1,
                            orientation,
                            GetNextObjectId());

                        dynamicObjects.push_back(t);
                    }
                    break;
                case OT_Waypoint:
                    {
                        Vector2 p1;
                        int nextWaypoint;
                        sscanf_s(buf, "%d %f %f %d", &objType, &p1.x, &p1.y, &nextWaypoint);
                        std::shared_ptr<Waypoint> w = std::make_shared<Waypoint>(p1, nextWaypoint);
                        w->id = GetNextObjectId();

                        waypoints.push_back(w);
                    }
                    break;
                default:
                    break;
                };
            }
        } while (!feof(file));

        if (file)
        {
            fclose(file);
        }

        return true;
    }

    void Load(_In_ GameContext* context)
    {
        LoadFromFile(context, "proto3world.txt");
        arrowTexture = context->graphics->GetSprite(L"arrow.png");
    }

    void Update(_In_ GameContext* context)
    {
        // Update all dynamic objects and remove dead ones
        std::vector<std::shared_ptr<GameObject>>::iterator it = dynamicObjects.begin();
        while(it != dynamicObjects.end())
        {
            if((*it)->isDeleted)
            {
                it = dynamicObjects.erase(it);
            }
            else
            {
                (*it)->Update(context);
                ++it;
            }
        }
    }

    void Draw(_In_ GameContext* context)
    {
        // Render level line-rendere stuff in a single batched line operation.
        // This includes walls, waypoints, etc.

        context->graphics->BeginBatchedLines();

        // Render walls
        std::vector<std::shared_ptr<Wall>>::iterator sit = walls.begin();
        while(sit != walls.end())
        {
            DrawWall(context, (*sit));
            ++sit;
        }

        // Render waypoints
        std::vector<std::shared_ptr<Waypoint>>::iterator wit = waypoints.begin();
        Vector2 wpt(0.0f);
        Vector2 startwpt(0.0f);
        bool drawWaypointConnector = false;
        while(wit != waypoints.end())
        {
            if (wpt.x == 0 && wpt.y == 0)
            {
                wpt = (*wit)->position;
                startwpt = wpt;
            }
            else
            {
                drawWaypointConnector = true;
            }

            DrawWaypoint(context, (*wit));

            if (drawWaypointConnector)
            {
                context->graphics->DrawBatchedLine(wpt, (*wit)->position, (startwpt == wpt) ? Colors::LightGreen : Colors::Green);
                wpt = (*wit)->position;
            }

            ++wit;
        }
        context->graphics->DrawBatchedLine(wpt, startwpt, Colors::Red);

        context->graphics->EndBatchedLines();

        // Render dynamic objects
        std::vector<std::shared_ptr<GameObject>>::iterator it = dynamicObjects.begin();
        while(it != dynamicObjects.end())
        {
            switch((*it)->type)
            {
            case OT_LightSoldier:
                DrawSoldier(context, (*it));
                break;
            case OT_Player:
                DrawPlayer(context, (*it));
                break;
            default:
                break;
            }
            ++it;
        }
    }

    void DrawSoldier(_In_ GameContext* context, _In_ std::shared_ptr<GameObject> soldier)
    {
        context->graphics->DrawSpriteWithOrigin(arrowTexture, soldier->position.x, soldier->position.y, soldier->orientation, 1, Colors::Tan);
    }

    void DrawWaypoint(_In_ GameContext* context, _In_ std::shared_ptr<Waypoint> waypoint)
    {
        int waypointSize = 4;
        Vector2 pa(waypoint->position.x + waypointSize, waypoint->position.y);
        Vector2 pb(waypoint->position.x - waypointSize, waypoint->position.y);
        context->graphics->DrawBatchedLine(pa,pb, Colors::LightGreen);
        pa.x = waypoint->position.x;
        pa.y = waypoint->position.y + waypointSize;
        pb.x = waypoint->position.x;
        pb.y = waypoint->position.y - waypointSize;
        context->graphics->DrawBatchedLine(pa,pb, Colors::LightGreen);
    }

    void DrawWall(_In_ GameContext* context, _In_ std::shared_ptr<Wall> wall)
    {
        float midX = (float)((wall->p1.x + wall->p2.x)/2);
        float midY = (float)((wall->p1.y + wall->p2.y)/2);

        context->graphics->DrawBatchedLine(wall->p1, wall->p2);
        context->graphics->DrawBatchedLine(Vector2(midX, midY),  Vector2((float)(midX+(wall->normal.x * 5)), (float)(midY+(wall->normal.y * 5))));
    }

    void DrawPlayer(_In_ GameContext* context, _In_ std::shared_ptr<GameObject> player)
    {
        context->graphics->DrawSpriteWithOrigin(arrowTexture, player->position.x, player->position.y, player->orientation, 1, Colors::Gray);
    }

    int GetNextObjectId() { return objectId++; }

    std::vector<std::shared_ptr<Wall>> walls;
    std::vector<std::shared_ptr<GameObject>> dynamicObjects;
    std::vector<std::shared_ptr<Waypoint>> waypoints;

    int objectId;

    std::shared_ptr<Sprite> arrowTexture;
};

class Prototype4 : public Game
{
public:
    Prototype4();
    ~Prototype4();
    void OnLoadContent();
    void OnUpdate(float elapsedTime);
    void OnDraw();

    GameContext context;
};
