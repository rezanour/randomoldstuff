#include <GameObject.h>

class Soldier : public GDK::GameObject
{
public:
    static std::shared_ptr<GDK::GameObject> Create(_In_ const std::wstring& type);

    Soldier(_In_ const std::wstring& type, _In_ const std::wstring& id);
};
