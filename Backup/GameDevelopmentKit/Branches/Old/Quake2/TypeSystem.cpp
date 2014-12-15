#include "Precomp.h"

namespace Quake2
{
    static GameObjectType GameObjectTypes[] = 
    {
        // Pickups
        GameObjectType::Pickup(L"Health Pack"),
        GameObjectType::Pickup(L"Basic Ammo Clip"),

        // Enemies
        GameObjectType::Enemy(L"Soldier"),
    };

    static std::map<std::wstring, std::vector<GameObjectType>> CategoryTypeMap;

    void GDKAPI EnsureTypeSystemInitialized()
    {
        if (CategoryTypeMap.size() == 0)
        {
            for (size_t i = 0; i < _countof(GameObjectTypes); ++i)
            {
                CategoryTypeMap[GameObjectTypes[i].Category].push_back(GameObjectTypes[i]);
            }
        }
    }

    void GDKAPI GetCategories(_Inout_ std::vector<const wchar_t*>& categories)
    {
        EnsureTypeSystemInitialized();

        categories.clear();
        for (auto it = CategoryTypeMap.begin(); it != CategoryTypeMap.end(); ++it)
        {
            categories.push_back(it->first.c_str());
        }
    }

    void GDKAPI GetAllTypes(_Inout_ std::vector<GameObjectType>& types)
    {
        EnsureTypeSystemInitialized();

        types.clear();
        for (size_t i = 0; i < _countof(GameObjectTypes); ++i)
        {
            types.push_back(GameObjectTypes[i]);
        }
    }

    void GDKAPI GetTypesInCategory(_In_z_ const wchar_t* category, _Inout_ std::vector<GameObjectType>& types)
    {
        EnsureTypeSystemInitialized();

        types.clear();
        auto filteredTypes = CategoryTypeMap[category];
        for (size_t i = 0; i < filteredTypes.size(); ++i)
        {
            types.push_back(filteredTypes[i]);
        }
    }
}