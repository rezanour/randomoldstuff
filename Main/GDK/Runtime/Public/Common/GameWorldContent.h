#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <MathTypes.h>

namespace GDK
{
    class GameWorldContent : public RuntimeObject<GameWorldContent>
    {
    public:
#pragma pack(push, 1)

        struct ModelChunk
        {
            wchar_t geometry[64];
            wchar_t texture[64];
        };

        struct EntityProperty
        {
            wchar_t name[32];
            wchar_t value[128];
        };

        struct ModelEntry
        {
            uint32_t offset;        // relative to start of model data, in chunks
            uint32_t numChunks;
        };

        struct EntityEntry
        {
            uint32_t offset;        // relative to start of entity data, in properties
            uint32_t numProperties;
        };

#pragma pack(pop)

        static std::shared_ptr<GameWorldContent> Create(_Inout_ std::istream &stream);
        static std::shared_ptr<GameWorldContent> Create(
            _In_ const std::wstring& name,
            _In_ uint32_t numModels, _In_ const ModelEntry* modelEntries, 
            _In_ uint32_t numChunks, _In_ const ModelChunk* chunks, 
            _In_ uint32_t numEntities, _In_ const EntityEntry* entityEntries, 
            _In_ uint32_t numProperties, _In_ const EntityProperty* properties);

        const std::wstring& GetName() const;

        uint32_t GetNumModels() const;
        const ModelEntry* GetModelEntries() const;

        uint32_t GetNumModelChunks() const;
        const ModelChunk* GetModelChunks() const;

        uint32_t GetNumEntities() const;
        const EntityEntry* GetEntityEntries() const;

        uint32_t GetNumEntityProperties() const;
        const EntityProperty* GetEntityProperties() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        GameWorldContent(
            _In_ const std::wstring& name,
            _In_ uint32_t numModels, _Inout_ std::unique_ptr<ModelEntry[]>& modelEntries, 
            _In_ uint32_t numChunks, _Inout_ std::unique_ptr<ModelChunk[]>& chunks, 
            _In_ uint32_t numEntities, _Inout_ std::unique_ptr<EntityEntry[]>& entityEntries, 
            _In_ uint32_t numProperties, _Inout_ std::unique_ptr<EntityProperty[]>& properties);

        std::wstring _name;
        uint32_t _numModels;
        uint32_t _numChunks;
        uint32_t _numEntities;
        uint32_t _numProperties;
        std::unique_ptr<ModelEntry[]> _modelEntries;
        std::unique_ptr<EntityEntry[]> _entityEntries;
        std::unique_ptr<ModelChunk[]> _chunks;
        std::unique_ptr<EntityProperty[]> _properties;
    };
}
