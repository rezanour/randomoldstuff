#pragma once

class GameWorld;

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
        isLoaded(false),
        type(OT_None)
    {
    };

    virtual void LoadContent(_In_ GameContext* context)
    {
        UNREFERENCED_PARAMETER(context);
        isLoaded = true;
    }

    virtual void Update(_In_ GameContext* context) = 0;
    virtual void Draw(_In_ GameContext* context) = 0;

    Vector2 position;
    float orientation;
    BoundingSphere boundingSphere;
    int id;
    bool isDead;
    bool isLoaded;
    ObjectType type;
};

class Wall : public GameObject
{
public:
    Wall(Vector2 pt1, Vector2 pt2)
    {
        drawNormal = true;

        p1 = pt1;
        p2 = pt2;

        // Calculate wall normal
        Vector2 t = p2 - p1;
        t.Normalize();

        normal.x = -t.y;
        normal.y = t.x;
    }

    void Update(_In_ GameContext* context) {}

    void Draw(_In_ GameContext* context)
    {
        context->graphics->DrawBatchedLine(p1,p2);

        //render the normals if rqd
        if (drawNormal)
        {
          float midX = (float)((p1.x+p2.x)/2);
          float midY = (float)((p1.y+p2.y)/2);

          context->graphics->DrawBatchedLine(Vector2(midX, midY),  Vector2((float)(midX+(normal.x * 5)), (float)(midY+(normal.y * 5))));
        }
    }

    Vector2 p1;
    Vector2 p2;
    Vector2 normal;
    bool drawNormal;
};

class Waypoint : public GameObject
{
public:
    Waypoint(Vector2 pt1)
    {
        position = pt1;
        waypointSize = 4;
    }

    void Update(_In_ GameContext* context) {}

    void Draw(_In_ GameContext* context)
    {
        Vector2 pa(position.x + waypointSize, position.y);
        Vector2 pb(position.x - waypointSize, position.y);
        context->graphics->DrawBatchedLine(pa,pb, Colors::LightGreen);
        pa.x = position.x;
        pa.y = position.y + waypointSize;
        pb.x = position.x;
        pb.y = position.y - waypointSize;
        context->graphics->DrawBatchedLine(pa,pb, Colors::LightGreen);
    }

    int waypointSize;
};

class Player : public GameObject
{
public:
    Player(_In_ Vector2 position, float orientation, float boundingRadius, int id)
    {
        this->position = position;
        boundingSphere.Center = XMFLOAT3(position.x,position.y,0);
        boundingSphere.Radius = boundingRadius;
        this->orientation = orientation;
        this->id = id;
        type = OT_Player;
    };

    void LoadContent(_In_ GameContext* context)
    {
        texture = context->graphics->GetSprite(L"arrow.png");
        wchar_t textBuffer[100];
        swprintf_s(textBuffer, L"(player %d)", id);
        infoText = textBuffer;
        isLoaded = true;
    }

    void Update(_In_ GameContext* context)
    {

    }

    void Draw(_In_ GameContext* context)
    {
        context->graphics->DrawSpriteWithOrigin(texture, position.x, position.y, orientation, 1, Colors::Gray);
        context->graphics->DrawTextW(infoText, position.x, position.y);
    }

    std::shared_ptr<Sprite> texture;
    std::wstring infoText;
};

class Soldier : public GameObject
{
public:
    Soldier(_In_ Vector2 position, float orientation, float boundingRadius, int id, ObjectType type)
    {
        this->position = position;
        boundingSphere.Center = XMFLOAT3(position.x,position.y,0);
        boundingSphere.Radius = boundingRadius;
        this->orientation = orientation;
        this->id = id;
        this->type = type;
    };

    void LoadContent(_In_ GameContext* context)
    {
        texture = context->graphics->GetSprite(L"arrow.png");
        wchar_t textBuffer[100];
        swprintf_s(textBuffer, L"(soldier %d)", id);

        infoText = textBuffer;
        isLoaded = true;
    }

    void Update(_In_ GameContext* context)
    {

    }

    void Draw(_In_ GameContext* context)
    {
        switch(type)
        {
        case OT_LightSoldier:
            context->graphics->DrawSpriteWithOrigin(texture, position.x, position.y, orientation, 1, Colors::Tan);
            break;
        default:
            context->graphics->DrawSpriteWithOrigin(texture, position.x, position.y, orientation, 1, Colors::Yellow);
            break;
        }

        context->graphics->DrawTextW(infoText, position.x, position.y);
    }

    std::shared_ptr<Sprite> texture;
    std::wstring infoText;
};

class GameWorld
{
public:
    GameWorld() :
        objectId(1)
    {

    }

    bool LoadFromFile(_In_ GameContext* context, _In_ char* path)
    {
        dynamicObjects.clear();
        staticObjects.clear();
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
                        8.0f,
                        GetNextObjectId(),
                        OT_LightSoldier);

                        t->LoadContent(context);
                        dynamicObjects.push_back(t);
                    }
                    break;
                case OT_Wall:
                    {
                        Vector2 p1;
                        Vector2 p2;
                        sscanf_s(buf, "%d %f %f %f %f", &objType, &p1.x, &p1.y, &p2.x, &p2.y);
                        std::shared_ptr<GameObject> w = std::make_shared<Wall>(p1, p2);
                        w->id = GetNextObjectId();

                        w->LoadContent(context);
                        staticObjects.push_back(w);
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
                            8.0f,
                            GetNextObjectId());

                        t->LoadContent(context);
                        dynamicObjects.push_back(t);
                    }
                    break;
                case OT_Waypoint:
                    {
                        Vector2 p1;
                        sscanf_s(buf, "%d %f %f", &objType, &p1.x, &p1.y);
                        std::shared_ptr<Waypoint> w = std::make_shared<Waypoint>(p1);
                        w->id = GetNextObjectId();

                        w->LoadContent(context);
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
    }

    void Update(_In_ GameContext* context)
    {
        std::vector<std::shared_ptr<GameObject>>::iterator it = dynamicObjects.begin();
        while(it != dynamicObjects.end())
        {
            if((*it)->isDead)
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
        // Render static objects (static objects contain batched line draws, so the draw loop is wrapped by the begin/end)
        std::vector<std::shared_ptr<GameObject>>::iterator sit = staticObjects.begin();
        context->graphics->BeginBatchedLines();
        while(sit != staticObjects.end())
        {
            (*sit)->Draw(context);
            ++sit;
        }
        context->graphics->EndBatchedLines();

        std::vector<std::shared_ptr<Waypoint>>::iterator wit = waypoints.begin();
        context->graphics->BeginBatchedLines();
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
            (*wit)->Draw(context);

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
            (*it)->Draw(context);
            ++it;
        }
    }

    int GetNextObjectId() { return objectId++; }

    std::vector<std::shared_ptr<GameObject>> staticObjects;
    std::vector<std::shared_ptr<GameObject>> dynamicObjects;
    std::vector<std::shared_ptr<Waypoint>> waypoints;
    int objectId;
};

class Prototype3 : public Game
{
public:
    Prototype3();
    ~Prototype3();
    void OnLoadContent();
    void OnUpdate(float elapsedTime);
    void OnDraw();

    GameContext context;
};