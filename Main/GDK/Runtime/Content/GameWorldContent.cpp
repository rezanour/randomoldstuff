#include <GameWorldContent.h>
#include <GDKError.h>

namespace GDK
{
    const uint32_t GameWorldContentVersion = 0x20315747; // "GW1 "

    #pragma pack(push,1)

    struct GameWorldContentHeader
    {
        uint32_t version;
        wchar_t name[32];
        uint32_t numModels;
        uint32_t numEntities;
        uint32_t numModelChunks;
        uint32_t numEntityProperties;
    };

    //
    // GameWorld Content File Specification
    // 
    // [GameWorldContentHeader] <-- File header
    // [ModelEntry Buffer] (ModelEntry) for each model
    // [EntityEntry Buffer ] (EntityEntry) for each entity
    // [Model data] <-- indexed by the model entries
    // [Entity data] <-- indexed by the entity entries
    //

    #pragma pack(pop)

    std::shared_ptr<GameWorldContent> GameWorldContent::Create(_Inout_ std::istream &stream)
    {
        GameWorldContentHeader header = {};
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));

        CHECK_TRUE(header.version == GameWorldContentVersion);

        std::unique_ptr<ModelEntry[]> modelEntries(new ModelEntry[header.numModels]);
        stream.read(reinterpret_cast<char*>(modelEntries.get()), sizeof(ModelEntry) * header.numModels);

        std::unique_ptr<EntityEntry[]> entityEntries(new EntityEntry[header.numEntities]);
        stream.read(reinterpret_cast<char*>(entityEntries.get()), sizeof(EntityEntry) * header.numEntities);

        std::unique_ptr<ModelChunk[]> chunks(new ModelChunk[header.numModelChunks]);
        stream.read(reinterpret_cast<char*>(chunks.get()), sizeof(ModelChunk) * header.numModelChunks);

        std::unique_ptr<EntityProperty[]> properties(new EntityProperty[header.numEntityProperties]);
        stream.read(reinterpret_cast<char*>(properties.get()), sizeof(EntityProperty) * header.numEntityProperties);

        return std::shared_ptr<GameWorldContent>(GDKNEW GameWorldContent(
            header.name,
            header.numModels, modelEntries, 
            header.numModelChunks, chunks, 
            header.numEntities, entityEntries, 
            header.numEntityProperties, properties));
    }

    std::shared_ptr<GameWorldContent> GameWorldContent::Create(
        _In_ const std::wstring& name, 
        _In_ uint32_t numModels, _In_ const ModelEntry* modelEntries, 
        _In_ uint32_t numChunks, _In_ const ModelChunk* chunks, 
        _In_ uint32_t numEntities, _In_ const EntityEntry* entityEntries, 
        _In_ uint32_t numProperties, _In_ const EntityProperty* properties)
    {
        std::unique_ptr<ModelEntry[]> modelEntriesCopy(new ModelEntry[numModels]);
        memcpy_s(modelEntriesCopy.get(), sizeof(ModelEntry) * numModels, modelEntries, sizeof(ModelEntry) * numModels);

        std::unique_ptr<EntityEntry[]> entityEntriesCopy(new EntityEntry[numEntities]);
        memcpy_s(entityEntriesCopy.get(), sizeof(EntityEntry) * numEntities, entityEntries, sizeof(EntityEntry) * numEntities);

        std::unique_ptr<ModelChunk[]> chunksCopy(new ModelChunk[numChunks]);
        memcpy_s(chunksCopy.get(), sizeof(ModelChunk) * numChunks, chunks, sizeof(ModelChunk) * numChunks);

        std::unique_ptr<EntityProperty[]> propertiesCopy(new EntityProperty[numProperties]);
        memcpy_s(propertiesCopy.get(), sizeof(EntityProperty) * numProperties, properties, sizeof(EntityProperty) * numProperties);

        return std::shared_ptr<GameWorldContent>(GDKNEW GameWorldContent(
            name,
            numModels, modelEntriesCopy,
            numChunks, chunksCopy,
            numEntities, entityEntriesCopy,
            numProperties, propertiesCopy));
    }

    GameWorldContent::GameWorldContent(
        _In_ const std::wstring& name, 
        _In_ uint32_t numModels, _Inout_ std::unique_ptr<ModelEntry[]>& modelEntries, 
        _In_ uint32_t numChunks, _Inout_ std::unique_ptr<ModelChunk[]>& chunks, 
        _In_ uint32_t numEntities, _Inout_ std::unique_ptr<EntityEntry[]>& entityEntries, 
        _In_ uint32_t numProperties, _Inout_ std::unique_ptr<EntityProperty[]>& properties) :
        _name(name), _numModels(numModels), _numChunks(numChunks), _numEntities(numEntities), _numProperties(numProperties)
    {
        _modelEntries.swap(modelEntries);
        _chunks.swap(chunks);
        _entityEntries.swap(entityEntries);
        _properties.swap(properties);
    }

    void GameWorldContent::Save(_Inout_ std::ostream& stream)
    {
        GameWorldContentHeader header = {};
        header.version = GameWorldContentVersion;
        wcscpy_s(header.name, _name.c_str());
        header.numModels = _numModels;
        header.numEntities = _numEntities;
        header.numModelChunks = _numChunks;
        header.numEntityProperties = _numProperties;

        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<char*>(_modelEntries.get()), sizeof(ModelEntry) * _numModels);
        stream.write(reinterpret_cast<char*>(_entityEntries.get()), sizeof(EntityEntry) * _numEntities);
        stream.write(reinterpret_cast<char*>(_chunks.get()), sizeof(ModelChunk) * _numChunks);
        stream.write(reinterpret_cast<char*>(_properties.get()), sizeof(EntityProperty) * _numProperties);
    }

    const std::wstring& GameWorldContent::GetName() const
    {
        return _name;
    }

    uint32_t GameWorldContent::GetNumModels() const
    {
        return _numModels;
    }

    const GameWorldContent::ModelEntry* GameWorldContent::GetModelEntries() const
    {
        return _modelEntries.get();
    }

    uint32_t GameWorldContent::GetNumModelChunks() const
    {
        return _numChunks;
    }

    const GameWorldContent::ModelChunk* GameWorldContent::GetModelChunks() const
    {
        return _chunks.get();
    }

    uint32_t GameWorldContent::GetNumEntities() const
    {
        return _numEntities;
    }

    const GameWorldContent::EntityEntry* GameWorldContent::GetEntityEntries() const
    {
        return _entityEntries.get();
    }

    uint32_t GameWorldContent::GetNumEntityProperties() const
    {
        return _numProperties;
    }

    const GameWorldContent::EntityProperty* GameWorldContent::GetEntityProperties() const
    {
        return _properties.get();
    }
}
