#pragma once

class Entity;
struct DoorEntityTrackingInfo;
struct IGraphicsScene;
struct IBsp;
struct IGeometry;
struct ITexture;
struct BspTriangle;
struct PositionNormalTangentTextureVertex;
struct EntityPlacement;
enum class EntityClass;

class GameWorld : public BaseObject<GameWorld>
{
public:
    static std::shared_ptr<GameWorld> Create();

    void LoadScene(_In_ const UpdateContext& context, _In_z_ const wchar_t* filename, _In_ bool compileBsp);
    void LoadEntities(_In_ const UpdateContext& context, _In_z_ const wchar_t* filename);
    void AddEntity(_In_ std::shared_ptr<Entity> entity);
    void AddDoor(_In_ std::shared_ptr<Entity> entity, _In_ DoorEntityTrackingInfo doorInfo);
    void AddPointLight(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ Vector3 pos, _In_ float radius, _In_ const XMVECTOR color);

    void Update(_In_ const UpdateContext& context);

    void ToggleEntityRendering(_In_ std::shared_ptr<Entity> entity, _In_ bool render);

    std::shared_ptr<Entity> GameWorld::TriggerEntityIfIntersected(_In_ std::shared_ptr<Entity> entity, _In_ EntityClass entityClass);

    std::shared_ptr<Entity> CreateEntityFromEntityPlacement(_In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnSoldier(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnHealth(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnAmmo(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnTreasure(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnDoor(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnCeilingLight(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);
    std::shared_ptr<Entity> SpawnPushWall(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo, _In_ const char* texture);
    std::shared_ptr<Entity> SpawnPatrolDirChangeMarker(_In_ const UpdateContext& context, _In_ const EntityPlacement& entityInfo);

    std::shared_ptr<Entity> GetEntityAsTarget(_In_opt_ std::vector<std::shared_ptr<Entity>>* entities, _In_ std::shared_ptr<Entity> entity, _In_ EntityClass entityClass);
    std::shared_ptr<Entity> FindClosestEntity(_In_opt_ std::vector<std::shared_ptr<Entity>>* entities, _In_ std::shared_ptr<Entity> entity, _In_ EntityClass entityClass);
    std::shared_ptr<Entity> GetNextEntity(_In_ std::shared_ptr<Entity> entity, _In_ EntityClass entityClass);

    std::shared_ptr<IGraphicsScene> GetScene();
    std::shared_ptr<Entity> GetPlayerControlledEntity();
    std::vector<std::shared_ptr<Entity>> GetVisibleEntities(_In_ std::shared_ptr<Entity> entity);
    bool IsInLineOfSight(_In_ const XMFLOAT3& source, _In_ const XMFLOAT3& target);
    void ResolveMovement(_In_ const std::shared_ptr<Entity>& self, _Inout_ XMFLOAT3& position, _Inout_ XMFLOAT3& velocity, _In_ const XMFLOAT3& radius);

    static bool VerticesLoadedCallback(_In_ void* callerContext, _In_ const PositionNormalTangentTextureVertex* vertices, _In_ const uint32_t numVertices, _In_ const uint32_t* indices, _In_ const uint32_t numIndices, _In_ const Transform& transform);
    void OnVerticesLoadedCallback(_In_ const PositionNormalTangentTextureVertex* vertices, _In_ const uint32_t numVertices, _In_ const uint32_t* indices, _In_ const uint32_t numIndices, _In_ const Transform& transform);

private:
    GameWorld();
    void UpdateDoors(_In_ const UpdateContext& context);
    void ProcessAutoClosingDoors();
    void TriggerDoorIfIntersected(_In_ std::shared_ptr<Entity> entity, _In_ const UpdateContext& context);
    bool CanTriggerDoor(_In_ std::shared_ptr<Entity> entity);
    void TriggerEntityIfIntersected(_In_ std::shared_ptr<Entity> entity, _In_ std::shared_ptr<Entity> entityTrigger, _In_ const UpdateContext& context);

    std::shared_ptr<ITexture> LoadTexture(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const XMVECTORF32 color);
    std::shared_ptr<ITexture> LoadTexture(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const char* name);
    std::shared_ptr<IGeometry> LoadGeometry(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ const char* name, _In_ const float targetHeight);
    void ClearCaches();

private:
    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Entity>> _solidEntities;
    std::vector<std::shared_ptr<Entity>> _sensorEntities;
    std::shared_ptr<IGraphicsScene> _scene;
    std::shared_ptr<Entity> _playerControlled;
    std::map<std::shared_ptr<Entity>, std::shared_ptr<DoorEntityTrackingInfo>> _doorMap;
    std::shared_ptr<IBsp> _sceneBsp;
    std::vector<BspTriangle> _bspTriangles;

    std::map<std::string, std::shared_ptr<IGeometry>> _geoCache;
    std::map<std::string, std::shared_ptr<ITexture>> _texCache;
};