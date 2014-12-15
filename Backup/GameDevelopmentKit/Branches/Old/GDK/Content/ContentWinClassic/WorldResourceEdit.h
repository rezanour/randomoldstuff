#pragma once

namespace GDK {
namespace Content {

    class WorldResourceEdit : public RuntimeObject<IWorldResourceEdit, IWorldResource>
    {
    public:
        WorldResourceEdit();

        // IPersistResource
        virtual HRESULT GDKAPI Save(_In_ GDK::IStream* output);
        virtual HRESULT GDKAPI Load(_In_ GDK::IStream* input);
        virtual HRESULT GDKAPI SetName(_In_ const wchar_t* name);

        // IWorldResourceEdit
        virtual HRESULT GDKAPI CreateWorldResource(_COM_Outptr_ IWorldResource** resource);

        // IWorldResource
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetWorldName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetObjects(_Out_ uint32_t* count, _Out_ const WORLD_RESOURCE_OBJECT** objects);
        
    private:
        ~WorldResourceEdit();

        WORLD_RESOURCE_FILEHEADER _header;
        std::wstring _resourceName;
        std::vector<WORLD_RESOURCE_OBJECT> _gameObjects;
    };

} // Content
} // GDK
