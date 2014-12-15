#ifndef _TYPESYSTEM_H_
#define _TYPESYSTEM_H_

namespace Quake2
{
    struct GameObjectType
    {
        const wchar_t* Category;
        const wchar_t* Type;

    protected:
        GameObjectType(_In_z_ const wchar_t* category, _In_z_ const wchar_t* type) :
            Category(category),
            Type(type)
        {
        }

    public:
        static GameObjectType Pickup(_In_z_ const wchar_t* type)
        {
            return GameObjectType(L"Pickup", type);
        }

        static GameObjectType Enemy(_In_z_ const wchar_t* type)
        {
            return GameObjectType(L"Enemy", type);
        }

    };

    void GDKAPI GetCategories(_Inout_ std::vector<const wchar_t*>& categories);
    void GDKAPI GetAllTypes(_Inout_ std::vector<GameObjectType>& types);
    void GDKAPI GetTypesInCategory(_In_z_ const wchar_t* category, _Inout_ std::vector<GameObjectType>& types);

} // Quake2

#endif // _TYPESYSTEM_H_