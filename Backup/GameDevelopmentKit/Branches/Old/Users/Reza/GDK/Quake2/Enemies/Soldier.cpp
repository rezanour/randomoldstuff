#include "Soldier.h"

std::shared_ptr<GDK::GameObject> Soldier::Create(_In_ const std::wstring& type)
{
    return std::shared_ptr<Soldier>(new(__FILEW__, __LINE__) Soldier(type, L"new soldier"));
}

Soldier::Soldier(_In_ const std::wstring& type, _In_ const std::wstring& id) :
    GDK::GameObject(type, id)
{
}
