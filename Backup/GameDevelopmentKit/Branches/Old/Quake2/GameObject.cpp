#include "Precomp.h"
#include "GameObject.h"

using namespace GDK;
using Microsoft::WRL::ComPtr;

namespace Quake2
{

//
// Game Object Factory Table
//
// To add new creatable types, add an entry to the table below
//

static struct
{
    const wchar_t* Type;
    ComPtr<GameObject> (GDKAPI *pfnCreationMethod)(_In_ const std::wstring& type, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content);
} g_GameObjectFactories[] =
    {
        { L"enemies\\soldier.object", Enemy::Create }
    };

ComPtr<GameObject> GDKAPI GameObject::Create(_In_ const std::wstring& type, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content)
{
    CHECK_IS_TRUE(!type.empty(), E_INVALIDARG);

    ComPtr<GameObject> gameObject;

    for (size_t i = 0; i < _countof(g_GameObjectFactories); ++i)
    {
        auto factory = g_GameObjectFactories[i];

        if (type.compare(factory.Type) == 0)
        {
            auto pfn = factory.pfnCreationMethod;
            CHECK_NOT_NULL(pfn, HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

            gameObject = pfn(type, graphics, content);
            break;
        }
    }

    CHECK_NOT_NULL(gameObject, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    gameObject->Initialize();
    return gameObject;
}

GameObject::GameObject(_In_ const std::wstring& type) :
    _type(type),
    _rotation(0.0f)
{
    _properties[L"Id"] = Make<StringProperty>(L"Id", &_id);
    _properties[L"Type"] = Make<StringProperty>(L"Type", &_type);
}

GameObject::~GameObject()
{
}

void GameObject::Initialize()
{
    ConfigureProperties(_properties);
}

void GameObject::UpdateWorld()
{
    _worldMatrix = Matrix::Multiply(Matrix::CreateRotationY(_rotation), Matrix::CreateTranslation(_position));
}

size_t GDKAPI GameObject::GetPropertyCount() const
{
    return _properties.size();
}

HRESULT GDKAPI GameObject::GetProperty(_In_z_ const wchar_t* name, _COM_Outptr_ IProperty** prop)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(prop, E_POINTER);
    *prop = nullptr;

    CHECK_NOT_NULL(name, E_INVALIDARG);

    auto entry = _properties.find(name);
    CHECK_IS_TRUE(entry != _properties.end(), HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    CHECKHR(entry->second.CopyTo(prop));

    MODULE_GUARD_END
}

HRESULT GDKAPI GameObject::GetPropertyByIndex(_In_ size_t index, _COM_Outptr_ IProperty** prop)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(prop, E_POINTER);
    *prop = nullptr;

    CHECK_IS_TRUE(index < _properties.size(), E_INVALIDARG);

    PropertyMap::iterator it = _properties.begin();
    while (index-- != 0) ++it;
    CHECKHR(it->second.CopyTo(prop));

    MODULE_GUARD_END
}

std::wstring GDKAPI GameObject::GetStringFromStream(_In_ GDK::IStream* stream)
{
    ULONG read = 0;
    size_t length = 0;
    CHECKHR(stream->Read(&length, sizeof(length), &read));
    std::unique_ptr<wchar_t[]> buffer(new wchar_t[length + 1]);
    CHECKHR(stream->Read(buffer.get(), static_cast<ULONG>(sizeof(wchar_t) * length), &read));
    buffer.get()[length] = L'\0';
    return std::wstring(buffer.get());
}

void GDKAPI GameObject::WriteStringToStream(_In_ GDK::IStream* stream, _In_ const std::wstring& value)
{
    ULONG written = 0;
    size_t length = value.size();
    CHECKHR(stream->Write(&length, sizeof(length), &written));
    CHECKHR(stream->Write(value.c_str(), static_cast<ULONG>(sizeof(wchar_t) * length), &written));
}

Microsoft::WRL::ComPtr<GameObject> GDKAPI GameObject::Load(_In_ GDK::IStream* stream, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content)
{
    ULONG read = 0;

    // Read in type, id, position, and rotation
    std::wstring type = GetStringFromStream(stream);
    auto gameObject = Create(type, graphics, content);
    gameObject->_id = GetStringFromStream(stream);
    CHECKHR(stream->Read(&gameObject->_position, sizeof(gameObject->_position), &read));
    CHECKHR(stream->Read(&gameObject->_rotation, sizeof(gameObject->_rotation), &read));
    gameObject->UpdateWorld();
    return gameObject;
}

void GDKAPI GameObject::Save(_In_ GDK::IStream* stream)
{
    ULONG written = 0;

    // Save out the type, id, position, and rotation
    WriteStringToStream(stream, _type);
    WriteStringToStream(stream, _id);
    CHECKHR(stream->Write(&_position, sizeof(_position), &written));
    CHECKHR(stream->Write(&_rotation, sizeof(_rotation), &written));
}

} // Quake2
