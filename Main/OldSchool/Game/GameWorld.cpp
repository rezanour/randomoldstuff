#include "Precomp.h"
#include "Contexts.h"
#include "transform.h"
#include "Texture.h"
#include "Light.h"
#include "Geometry.h"
#include "light.h"
#include "Model.h"
#include "AssetLoading.h"
#include "GraphicsSystem.h"
#include "GraphicsScene.h"
#include "Weapon.h"
#include "Behaviors.h"
#include "Doors.h"
#include "Bsp.h"
#include "GameWorld.h"

#include "Entity.h"

#include <DirectXCollision.h>
#include <DirectXColors.h>

// Simplify entity placement in the world when
// read from a file
struct EntityPlacement
{
    Vector3 Position;
    Vector3 BoundingBox;
    float Rotation;
    EntityClass EntityClass;
    CollisionType CollisionType;
    bool IsValid;
};

std::shared_ptr<GameWorld> GameWorld::Create()
{
    return std::shared_ptr<GameWorld>(new GameWorld);
}

GameWorld::GameWorld()
{

}

_Use_decl_annotations_
void GameWorld::LoadScene(const UpdateContext& context, const wchar_t* filename, bool compileBsp)
{
    auto assetLoader = AssetLoader::Create();
    assetLoader->SetVerticesLoadedCallback(this, VerticesLoadedCallback);
    _scene = assetLoader->LoadSceneFromFile(context.GraphicsSystem, filename);
    if (compileBsp)
    {
        _sceneBsp = GenerateBspFromTriangles(_bspTriangles);

        // dump these now, since we don't need them anymore and they take a ton of space
        _bspTriangles.clear();
    }
}

_Use_decl_annotations_
void GameWorld::LoadEntities(const UpdateContext& context, const wchar_t* filename)
{
    auto assetLoader = AssetLoader::Create();
    std::vector<uint8_t> buffer;
    AssetLoader::ReadFileToBuffer(filename, buffer);

    char* text = (char*)buffer.data();
    char* ctx = nullptr;
    char * line = nullptr;
    line = strtok_s(text, "\r\n", &ctx);
    while (line != nullptr)
    {
        char objType[32] = { 0 };
        if (line && line[0] != 0 && line[0] != '\n' && line[0] != '/')
        {
            sscanf_s(line, "%s", objType, (uint32_t)_countof(objType));
        }

        if (strlen(objType))
        {
            std::shared_ptr<Entity> entity;
            EntityPlacement entityInfo;
            entityInfo.IsValid = false;

            if (_strcmpi(objType, "start") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                // let's play as the soldier character, so spawn a soldier as the player :)
                entity = SpawnSoldier(context, entityInfo);
                entity->SetBehavior(PlayerControlledBehavior::Create());
            }
            else if (_strcmpi(objType, "guard") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                entity = SpawnSoldier(context, entityInfo);
            }
            else if (_strcmpi(objType, "pguard") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                entity = SpawnSoldier(context, entityInfo);
                entity->SetBehavior(PatrolBehavior::Create());
            }
            else if (_strcmpi(objType, "door") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnDoor(context, entityInfo);
            }
            else if (_strcmpi(objType, "ammo") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnAmmo(context, entityInfo);
            }
            else if (_strcmpi(objType, "patroldirchange") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnPatrolDirChangeMarker(context, entityInfo);
            }
            else if (_strcmpi(objType, "pushwall") == 0)
            {
                char textureName[32] = { 0 };
                sscanf_s(line, "%s %f %f %f %f %s", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation, textureName, (uint32_t)_countof(textureName));
                SpawnPushWall(context, entityInfo, textureName);
            }
            else if (_strcmpi(objType, "goldcup") == 0 || _strcmpi(objType, "goldcross") == 0 || _strcmpi(objType, "goldchest") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnTreasure(context, entityInfo);
            }
            else if (_strcmpi(objType, "greenceilinglight") == 0 || _strcmpi(objType, "chandilier") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnCeilingLight(context, entityInfo);
            }
            else if (_strcmpi(objType, "healthkit") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnHealth(context, entityInfo);
            }
            else if (_strcmpi(objType, "tvdinner") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnHealth(context, entityInfo);
            }
            else if (_strcmpi(objType, "dogfood") == 0)
            {
                sscanf_s(line, "%s %f %f %f %f ", objType, (uint32_t)_countof(objType), &entityInfo.Position.x, &entityInfo.Position.y, &entityInfo.Position.z, &entityInfo.Rotation);
                SpawnHealth(context, entityInfo);
            }

            // Add a debug bounding model to the debug scene
            //AddBoundingBoxModelForEntity(graphics, entity);
        }

        line = strtok_s(nullptr, "\r\n", &ctx);
    }

    // Clear any caches built during loading
    ClearCaches();
}

_Use_decl_annotations_
void GameWorld::AddDoor(std::shared_ptr<Entity> entity, DoorEntityTrackingInfo doorInfo)
{
    // add door entity to special tracking and special handling
    _doorMap[entity] = std::shared_ptr<DoorEntityTrackingInfo>(new DoorEntityTrackingInfo);
    _doorMap[entity]->ClosePositionTransform = doorInfo.ClosePositionTransform;
    _doorMap[entity]->ElapsedTimeToComplete = doorInfo.ElapsedTimeToComplete;
    _doorMap[entity]->OpenCloseElapsedTime = doorInfo.OpenCloseElapsedTime;
    _doorMap[entity]->State = doorInfo.State;
    _doorMap[entity]->BoundingBox = BoundingBox(entity->GetTransform().GetPosition(), Vector3(6, 6, 6));
}

_Use_decl_annotations_
void GameWorld::AddEntity(std::shared_ptr<Entity> entity)
{
    _entities.push_back(entity);

    auto collisionType = entity->GetCollisionType();
    if (collisionType == CollisionType::Solid)
    {
        _solidEntities.push_back(entity);
    }
    else if (collisionType == CollisionType::Sensor)
    {
        _sensorEntities.push_back(entity);
    }

    // If there is a model for this entity, add it to the scene.
    // Model's are optional.  Not all entities have visual components.
    // Sound emitters, triggers, waypoint/paths, etc. do not have models
    // or visuals.
    if (entity->GetModel())
    {
        _scene->AddModel(entity->GetModel());
    }
}

_Use_decl_annotations_
void GameWorld::UpdateDoors(const UpdateContext& context)
{
    // Process any auto closing door logic
    ProcessAutoClosingDoors();

    // Update door animations.
    // TODO: This code could be moved and performed in the door entity's update method.
    //       Every door entity is already in the master entities list.  They should either
    //       stay in the list, or be removed to reduce the number of total entities being
    //       updated.  Some more thinking needs to be done here.
    for (auto door : _doorMap)
    {
        auto doorInfo = door.second;
        auto theDoor = door.first;

        // If the door has already been triggered and is in its full open state
        // keep it that way.  There is something blocking the door.
        if (doorInfo->Triggered && doorInfo->State == DoorEntityState::Opened)
        {
            doorInfo->Triggered = false;
            continue;
        }

        // If the door is not in the closed state, then it needs to be processed
        if (doorInfo->State != DoorEntityState::Closed)
        {
            // Start and End positions are calculated using door's closed position.
            // Depending on the state of the door, the Start and End positions will 
            // be adjusted accordingly.
            Vector3 endPos(doorInfo->ClosePositionTransform.GetPositionV());
            Vector3 startPos(doorInfo->ClosePositionTransform.GetPositionV());

            switch(doorInfo->State)
            {
                // If door is closed or in the process of being opened,
                // set the end position to be fully opened.
                case DoorEntityState::Closed:
                case DoorEntityState::Opening:
                    endPos -= doorInfo->ClosePositionTransform.GetRightV() * 8;
                    break;

                // If the door is opened or in the process of being closed,
                // set the start position to the fully opened.
                case DoorEntityState::Opened:
                case DoorEntityState::Closing:
                    startPos -= doorInfo->ClosePositionTransform.GetRightV() * 8;
                    break;

                default:
                    break;
            }

            // Accumulate the opened/closed elapsed time
            doorInfo->OpenCloseElapsedTime += context.ElapsedTime;

            // LERP the door positions begin tracked in the door information struct
            Vector3 currPos = Vector3::Lerp(startPos, endPos, (float)(doorInfo->OpenCloseElapsedTime/doorInfo->ElapsedTimeToComplete));

            // Move the door model to the calculated current position
            Transform t = theDoor->GetTransform();
            t.SetPosition(currPos);
            theDoor->SetTransform(t);

            // If the open/closed target elapsed time has been reached or exceeded,
            // the door state needs to be reset.
            if (doorInfo->OpenCloseElapsedTime >= doorInfo->ElapsedTimeToComplete)
            {
                // Reset the timer accumulator
                doorInfo->OpenCloseElapsedTime = 0.0f;

                // If the door was opening when the timer was reached,
                // it should now be opened.
                if (doorInfo->State == DoorEntityState::Opening)
                {
                    doorInfo->State = DoorEntityState::Opened;
                }

                // If the door was closing when the timer was reached,
                // it should now be closed.
                else if (doorInfo->State == DoorEntityState::Closing)
                {
                    doorInfo->State = DoorEntityState::Closed;
                }
            }
        }

        // Reset the triggered flag for next frame
        doorInfo->Triggered = false;
    }
}

_Use_decl_annotations_
void GameWorld::Update(const UpdateContext& context)
{
    std::vector<EntityPlacement> ammoDrops;

    for (auto entity : _entities)
    {
        if (!entity->IsDead())
        {
            // Update the entity
            entity->Update(context);

            // Check if the triggered a door
            TriggerDoorIfIntersected(entity, context);

            // Check if the player has triggered another entity
            TriggerEntityIfIntersected(_playerControlled, entity, context);
        }
        else
        {
            if (entity->GetModel())
            {
                _scene->RemoveModel(entity->GetModel());
                entity->SetModel(nullptr);
                if (entity->GetClass() == EntityClass::Soldier)
                {
                    EntityPlacement entityInfo;
                    entityInfo.Position = entity->GetTransform().GetPositionV();
                    ammoDrops.push_back(entityInfo);
                }
            }
        }
    }

    UpdateDoors(context);

    // Drop any ammo for characters that have died
    for (auto info : ammoDrops)
    {
        SpawnAmmo(context, info);
    }
}

void GameWorld::ProcessAutoClosingDoors()
{
    for (auto door : _doorMap)
    {
        auto doorInfo = door.second;

        // If the door has not yet been triggered, check for auto close logic.
        if (!doorInfo->Triggered)
        {
            // If the door is in the 'opened' state, it needs to be closed.
            // Set the door's state to 'closing'.
            if (doorInfo->State == DoorEntityState::Opened)
            {
                doorInfo->State = DoorEntityState::Closing;
            }
        }
    }
}

_Use_decl_annotations_
bool GameWorld::CanTriggerDoor(std::shared_ptr<Entity> entity)
{
    EntityClass entityClass = entity->GetClass();
    return ((entityClass == EntityClass::Dog) ||
            (entityClass == EntityClass::Player) ||
            (entityClass == EntityClass::Soldier) ||
            (entityClass == EntityClass::SS_Soldier));
}

_Use_decl_annotations_
void GameWorld::AddPointLight(const std::shared_ptr<IGraphicsSystem>& graphics, Vector3 pos, float radius, const XMVECTOR color)
{
    XMFLOAT3 c;
    XMStoreFloat3(&c, color);
    std::shared_ptr<ILight> light = graphics->CreateLight(LightType::Point, c, radius);
    light->SetPosition(pos);
    _scene->AddLight(light);
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::TriggerEntityIfIntersected(std::shared_ptr<Entity> entity, EntityClass entityClass)
{
    std::shared_ptr<Entity> trigger;
    for (auto e : _entities)
    {
        if (e != entity && entityClass == e->GetClass())
        {
            if (entity->GetAABB().Overlaps(e->GetAABB()))
            {
                trigger = e;
                return trigger;
            }
        }
    }

    return trigger;
}

_Use_decl_annotations_
void GameWorld::TriggerEntityIfIntersected(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> entityTrigger, const UpdateContext& context)
{
    UNREFERENCED_PARAMETER(context);

    if (entity == entityTrigger)
    {
        return;
    }

    if (!entity->GetAABB().Overlaps(entityTrigger->GetAABB()))
    {
        return;
    }

    EntityClass entityClass = entityTrigger->GetClass();
    switch (entityClass)
    {
        /*
        case EntityClass::PatrolDirChange:
        {
            Transform et = entityTrigger->GetTransform();
            entity->GetTransform().SetOrientation(et.GetOrientation());
        }
        break;
        */
        case EntityClass::Ammo:
        {
            entity->GetWeapon()->AddAmmo(7);
            entityTrigger->Die();
        }
        break;
        case EntityClass::Health:
        {
            if (entity->AddHealth(15))
            {
                entityTrigger->Die();
            }
        }
        break;
        case EntityClass::Treasure:
        {
            entity->AddScore(500);
            entityTrigger->Die();
        }
        break;
        case EntityClass::Player:
        case EntityClass::Soldier:
        case EntityClass::SS_Soldier:
        case EntityClass::Dog:
        case EntityClass::Door:
        case EntityClass::CeilingLight:
        default:
            break;
    }
}

_Use_decl_annotations_
void GameWorld::TriggerDoorIfIntersected(std::shared_ptr<Entity> entity, const UpdateContext& context)
{
    // Do not attempt to process entities that cannot possible trigger doors.
    if (!CanTriggerDoor(entity))
    {
        return;
    }

    UNREFERENCED_PARAMETER(context);

    for (auto door : _doorMap)
    {
        auto doorInfo = door.second;
        auto theDoor = door.first;

        // If the door has not yet been triggered, check for triggering entities.
        if (!doorInfo->Triggered)
        {
            // If the entity intersected with the door bounding box, trigger the door to open
            // by setting the state to 'opening'
            if (entity->GetAABB().Overlaps(AABB(XMLoadFloat3(&doorInfo->BoundingBox.Center), XMLoadFloat3(&doorInfo->BoundingBox.Extents))))
            {
                // If the door is closed or closing, force it to open again.
                if (doorInfo->State == DoorEntityState::Closed ||
                    doorInfo->State == DoorEntityState::Closing)
                {
                    doorInfo->State = DoorEntityState::Opening;
                }

                // Ensure to set the triggered flag to avoid processing the same door again.
                // Multiple entities can trigger the same door.  First entity in the list wins,
                // but the result is the same.  Door entere 'opening' state.
                doorInfo->Triggered = true;
            }
        }
    }
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::FindClosestEntity(std::vector<std::shared_ptr<Entity>>* entities, std::shared_ptr<Entity> entity, EntityClass entityClass)
{
    std::shared_ptr<Entity> closestEntity;
    float distance = 0;
    std::vector<std::shared_ptr<Entity>>* entitiesToSearch = (entities == nullptr) ? &_entities : entities;

    for (auto e : *entitiesToSearch)
    {
        if (e != entity && e->GetClass() == entityClass)
        {
            float d = Vector3::Distance(entity->GetTransform().GetPositionV(), e->GetTransform().GetPositionV());

            if (distance == 0 || distance > d)
            {
                closestEntity = e;
                distance = d;
            }
        }
    }

    return closestEntity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::GetNextEntity(std::shared_ptr<Entity> entity, EntityClass entityClass)
{
    std::shared_ptr<Entity> nextEntity;
    bool foundNextEntity = false;
    for (auto e : _entities)
    {
        if (foundNextEntity && e->GetClass() == entityClass)
        {
            nextEntity = e;
            break;
        }

        if (e == entity)
        {
            foundNextEntity = true;
        }
    }

    if (nextEntity == nullptr)
    {
        nextEntity = GetNextEntity(_entities[0], entityClass);
    }

    return nextEntity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::GetEntityAsTarget(std::vector<std::shared_ptr<Entity>>* entities, std::shared_ptr<Entity> entity, EntityClass entityClass)
{
    std::shared_ptr<Entity> targetEntity;
    float distance = 0;
    float lastDistance = 0;
    std::vector<std::shared_ptr<Entity>>* entitiesToSearch = (entities == nullptr) ? &_entities : entities;
    Vector3 dir = entity->GetTransform().GetForwardV();
    dir.Normalize();

    for (auto e : *entitiesToSearch)
    {
        if (e != entity && (entityClass == EntityClass::Any || e->GetClass() == entityClass))
        {
            if (e->GetAABB().Raycast(entity->GetTransform().GetPositionV(), dir, distance) && distance <= entity->GetMaxSightDistance())
            {
                if (lastDistance == 0)
                {
                    lastDistance = distance;
                    targetEntity = e;
                }
                else if (distance < lastDistance)
                {
                    lastDistance = distance;
                    targetEntity = e;
                }
            }
        }
    }

    return targetEntity;
}

std::shared_ptr<IGraphicsScene> GameWorld::GetScene()
{
    return _scene;
}

std::shared_ptr<Entity> GameWorld::GetPlayerControlledEntity()
{
    if (_playerControlled == nullptr)
    {
        for (auto e : _entities)
        {
            if (e->GetBehavior())
            {
                if (e->GetBehavior()->GetClass() == BehaviorClass::PlayerControlled)
                {
                    _playerControlled = e;
                    break;
                }
            }
        }
    }

    return _playerControlled;
}

_Use_decl_annotations_
std::vector<std::shared_ptr<Entity>> GameWorld::GetVisibleEntities(std::shared_ptr<Entity> entity)
{
    std::vector<std::shared_ptr<Entity>> visibleEntities;

    BoundingFrustum frustum;
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(entity->GetFieldOfView()), 800.0f / 600.0f, 0.1f, entity->GetMaxSightDistance());
    BoundingFrustum::CreateFromMatrix(frustum, projection);
    frustum.Origin = entity->GetTransform().GetPosition();
    frustum.Orientation = entity->GetTransform().GetOrientation();

    for (auto e : _entities)
    {
        XMFLOAT3 mins = e->GetAABB().GetMin();
        XMFLOAT3 maxs = e->GetAABB().GetMax();
        Vector3 center = (Vector3(&mins.x) + Vector3(&maxs.x)) * 0.5f;
        Vector3 extents = Vector3(&maxs.x) - center;
        if (frustum.Contains(BoundingBox(center, extents)))
        {
            if (IsInLineOfSight(entity->GetTransform().GetPosition(), e->GetTransform().GetPosition()))
            {
                visibleEntities.push_back(e);
            }
        }
    }

    return visibleEntities;
}

_Use_decl_annotations_
bool GameWorld::VerticesLoadedCallback(void* callerContext, const PositionNormalTangentTextureVertex* vertices, const uint32_t numVertices, const uint32_t* indices, const uint32_t numIndices, const Transform& transform)
{
    GameWorld* world = (GameWorld*)callerContext;
    if (world)
    {
        world->OnVerticesLoadedCallback(vertices, numVertices, indices, numIndices, transform);
    }

    return true;
}

_Use_decl_annotations_
void GameWorld::OnVerticesLoadedCallback(const PositionNormalTangentTextureVertex* vertices, const uint32_t numVertices, const uint32_t* indices, const uint32_t numIndices, const Transform& transform)
{
    UNREFERENCED_PARAMETER(numVertices);

    XMMATRIX world = transform.GetWorldV();

    const uint32_t* index = indices;
    for (uint32_t i = 0; i < numIndices; i += 3)
    {
        auto& v0 = vertices[*index++];
        auto& v1 = vertices[*index++];
        auto& v2 = vertices[*index++];

        XMFLOAT3 vv0, vv1, vv2;
        XMStoreFloat3(&vv0, XMVector3Transform(XMLoadFloat3(&v0.Position), world));
        XMStoreFloat3(&vv1, XMVector3Transform(XMLoadFloat3(&v1.Position), world));
        XMStoreFloat3(&vv2, XMVector3Transform(XMLoadFloat3(&v2.Position), world));

        BspTriangle triangle(vv0, vv1, vv2);

        _bspTriangles.push_back(triangle);
    }
}

_Use_decl_annotations_
bool GameWorld::IsInLineOfSight(const XMFLOAT3& source, const XMFLOAT3& target)
{
    if (_sceneBsp)
    {
        ClipResult result;
        return _sceneBsp->ClipLineSegment(XMLoadFloat3(&source), XMLoadFloat3(&target), result);
    }
    return true;
}

_Use_decl_annotations_
void GameWorld::ResolveMovement(const std::shared_ptr<Entity>& self, XMFLOAT3& position, XMFLOAT3& velocity, const XMFLOAT3& radius)
{
    ProfileMark profile(CodeTag::Collision);

    if (_sceneBsp == nullptr)
    {
        XMStoreFloat3(&position, XMLoadFloat3(&position) + XMLoadFloat3(&velocity));
    }
    else
    {
        CollisionQuery query;

        query.WorldPosition = XMLoadFloat3(&position);
        query.WorldMovement = XMLoadFloat3(&velocity);
        query.eRadius = XMLoadFloat3(&radius);

        // ellipsoid space
        query.BasePoint = query.WorldPosition / query.eRadius;
        query.Velocity = query.WorldMovement / query.eRadius;
        query.VelocityLen2 = XMVector3LengthSq(query.Velocity);

        query.Collided = false;

        int32_t iterations = 10;
        bool done = false;

        while (iterations-- > 0 && !done)
        {
            // Check level
            _sceneBsp->CheckEllipsoid(query);

            // TODO: NEED BROADPHASE. THIS IS REALLY SLOW!!!!

            AABB velBox = AABB(query.BasePoint, self->GetAABB().GetExtentsV());
            velBox.ExpandBy(query.Velocity);

            // Check entities
            for (auto& entity : _solidEntities)
            {
                if (entity != self)
                {
                    AABB aabb = entity->GetAABB();
                    aabb = AABB(aabb.GetCenterV() / query.eRadius, aabb.GetExtentsV() / query.eRadius);
                    if (velBox.Overlaps(aabb))
                    {
                        TestEllipsoidAABB(query, aabb);
                    }
                }
            }

            done = ResolveCollision(query);
        }

#if _DEBUG
        if (iterations <= 0)
        {
            OutputDebugString(L"Exceeded ResolveMovement iteration count!\n");
        }
#endif

        // Convert back to world space
        query.WorldPosition = query.BasePoint * query.eRadius;
        query.WorldMovement = query.Velocity * query.eRadius;

        XMStoreFloat3(&position, query.WorldPosition);
        XMStoreFloat3(&velocity, query.WorldMovement);
    }
}

_Use_decl_annotations_
void GameWorld::ToggleEntityRendering(std::shared_ptr<Entity> entity, bool render)
{
    if (render)
    {
        _scene->RemoveModel(entity->GetModel());
        _scene->AddModel(entity->GetModel());
    }
    else
    {
        _scene->RemoveModel(entity->GetModel());
    }
}

_Use_decl_annotations_
std::shared_ptr<ITexture> GameWorld::LoadTexture(const std::shared_ptr<IGraphicsSystem>& graphics, const XMVECTORF32 color)
{
    std::string key = "clr";
    const float* clr = color;
    key += std::to_string(clr[0]);
    key += std::to_string(clr[1]);
    key += std::to_string(clr[2]);

    if (_texCache.count(key) == 0)
    {
        _texCache[key] = graphics->CreateTexture(1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, color, sizeof(color), false);
    }

    return _texCache[key];
}

_Use_decl_annotations_
std::shared_ptr<ITexture> GameWorld::LoadTexture(const std::shared_ptr<IGraphicsSystem>& graphics, const char* name)
{
    if (_texCache.count(name) == 0)
    {
        auto assetLoader = AssetLoader::Create();
        _texCache[name] = assetLoader->LoadTextureFromFile(graphics, std::AnsiToWide(name).c_str(), false, nullptr);
    }

    return _texCache[name];
}

_Use_decl_annotations_
std::shared_ptr<IGeometry> GameWorld::LoadGeometry(const std::shared_ptr<IGraphicsSystem>& graphics, const char* name, const float targetHeight)
{
    std::string key = name;
    key += std::to_string(targetHeight);

    if (_geoCache.count(key) == 0)
    {
        auto assetLoader = AssetLoader::Create();
        _geoCache[key] = assetLoader->LoadGeometryFromGDKGeometryFile(graphics, std::AnsiToWide(name).c_str(), targetHeight, Transform());
    }

    return _geoCache[key];
}

void GameWorld::ClearCaches()
{
    _texCache.clear();
    _geoCache.clear();
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::CreateEntityFromEntityPlacement(const EntityPlacement& entityInfo)
{
    Transform transform;
    transform.SetPosition(entityInfo.Position);
    transform.Rotate(transform.GetUpV(), entityInfo.Rotation);

    // Create entity
    std::shared_ptr<Entity> entity = Entity::Create(entityInfo.EntityClass,transform, entityInfo.BoundingBox, entityInfo.CollisionType);
    entity->SetGameWorld(this->shared_from_this());

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnHealth(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::Health;
    info.BoundingBox = Vector3(1, 1, 1);
    info.Position.y = 1.0f;
    info.CollisionType = CollisionType::Sensor;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(10);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 1.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, Colors::Green));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    AddEntity(entity);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnTreasure(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::Treasure;
    info.BoundingBox = Vector3(1, 1, 1);
    info.Position.y = 1.0f;
    info.CollisionType = CollisionType::Sensor;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(500);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 1.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, Colors::Gold));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    AddEntity(entity);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnAmmo(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::Ammo;
    info.BoundingBox = Vector3(1, 1, 1);
    info.Position.y = 1.0f;
    info.CollisionType = CollisionType::Sensor;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(7);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 1.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, Colors::CornflowerBlue));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    AddEntity(entity);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnPatrolDirChangeMarker(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::PatrolDirChange;
    info.BoundingBox = Vector3(1, 1, 1);
    info.Position.y = 1.0f;
    info.CollisionType = CollisionType::Sensor;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(7);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 1.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, Colors::Red));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    AddEntity(entity);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnSoldier(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::Soldier;
    info.BoundingBox = Vector3(1.25f, 6, 1.25f);
    info.Position.y = 2.0f;
    info.CollisionType = CollisionType::Solid;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(100);
    entity->SetSpeed(SpeedType::MovementSpeed, 0.0f);
    entity->AddWeapon(Weapon::Create(WeaponClass::Pistol, 40, 7, 200, 1));

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "soldier.geometry", 6.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, "soldier_skin.pcx.texture"));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    AddEntity(entity);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnDoor(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::Door;
    info.BoundingBox = Vector3(4, 4, 4);
    info.Position.y = 4.0f;
    info.CollisionType = CollisionType::Solid;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(100);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_door.geometry", 8.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, "99.texture"));
    // exit DoorTexture = 104.texture

    // Set model on entity
    entity->SetModel(model);

    DoorEntityTrackingInfo doorInfo = {DoorEntityState::Closed, entity->GetTransform(), 0.0f, 1.0f, BoundingBox(), false};
    AddDoor(entity, doorInfo);

    // add entity to world
    AddEntity(entity);

    // add model only to world for rendering
    _scene->AddModel(model);

    return entity;
}

_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnPushWall(const UpdateContext& context, const EntityPlacement& entityInfo, const char* texture)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::PushWall;
    info.BoundingBox = Vector3(8, 8, 8);
    info.Position.y = 4.0f;
    info.CollisionType = CollisionType::Sensor;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(100);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 8.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, texture));

    // Set model on entity
    entity->SetModel(model);

    //DoorEntityTrackingInfo doorInfo = {DoorEntityState::Closed, entity->GetTransform(), 0.0f, 1.0f, BoundingBox(), false};
    //AddDoor(entity, doorInfo);

    // add entity to world
    AddEntity(entity);

    return entity;
}
_Use_decl_annotations_
std::shared_ptr<Entity> GameWorld::SpawnCeilingLight(const UpdateContext& context, const EntityPlacement& entityInfo)
{
    EntityPlacement info = entityInfo;
    info.EntityClass = EntityClass::CeilingLight;
    info.BoundingBox = Vector3(1, 1, 1);
    info.Position.y = 8.0f;
    info.CollisionType = CollisionType::None;

    std::shared_ptr<Entity> entity = CreateEntityFromEntityPlacement(info);

    entity->SetHealth(500);

    // Create model
    std::shared_ptr<IModel> model;
    model = context.GraphicsSystem->CreateModel(entity);
    model->SetGeometry(LoadGeometry(context.GraphicsSystem, "w3d_pushwall.geometry", 1.0f));
    model->SetTexture(TextureUsage::Diffuse, LoadTexture(context.GraphicsSystem, Colors::Yellow));

    // Set model on entity
    entity->SetModel(model);

    // add entity to world
    //AddEntity(entity);

    // add model to world
    _scene->AddModel(model);

    // Add lighting effects
    info.Position.y = 7.0f;
    AddPointLight(context.GraphicsSystem, info.Position, 3.0f, Colors::Orange);
    info.Position.y = 1.0f;
    AddPointLight(context.GraphicsSystem, info.Position, 6.0f, Colors::Orange);

    return entity;
}