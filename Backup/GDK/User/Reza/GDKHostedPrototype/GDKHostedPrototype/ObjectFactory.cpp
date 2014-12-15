#include "StdAfx.h"

using namespace GDK;

GDK_METHOD ObjectFactory::Create(_Out_ ObjectFactory** factory)
{
    HRESULT hr = S_OK;

    CHECKP(factory, E_POINTER);
    *factory = new ObjectFactory;
    CHECKP(*factory, E_OUTOFMEMORY);

Exit:
    return hr;
}

ObjectFactory::ObjectFactory()
{
}

ObjectFactory::~ObjectFactory()
{
}

GDK_METHOD ObjectFactory::CreateObject(_In_ void* typeInfo, _Out_ Object** object)
{
    HRESULT hr = S_OK;

    UNREFERENCED_PARAMETER(typeInfo);

    CHECKP(object, E_POINTER);

    *object = new Object;
    CHECKP(*object, E_OUTOFMEMORY);

    // HACK: hard coded type for now
    (*object)->_maxComponents = 10;
    (*object)->_components = new Component*[10];
    CHECKP((*object)->_components, E_OUTOFMEMORY);

    CameraComponent* camera = nullptr;
    CHECK(CameraComponent::Create(*object, CameraType_Perspective, &camera));
    CHECK((*object)->AddComponent(camera));

Exit:
    if (FAILED(hr))
    {
        SafeRelease(camera);
        SafeRelease(*object);
    }
    return hr;
}


GDK_METHOD GDK::ObjectTest()
{
    HRESULT hr = S_OK;

    Screen* screen = nullptr;
    IGameObject* dudes[10] = {0};
    IGameObject* dude = nullptr;

    CHECK(Screen::Create(L"Wolf3D - E1M1", &screen));

    for (int i = 0; i < _countof(dudes); ++i)
    {
        CHECK(screen->CreateObject(nullptr, &dudes[i]));
        CHECK(screen->AddObject(dudes[i]));
    }

    CHECK(screen->FindObject(dudes[4]->GetID(), &dude));

    CHECKB(dude->GetID() == dudes[4]->GetID(), E_FAIL);

    Object* dudeObj = dynamic_cast<Object*>(dude);
    Message msg;
    CHECK(dudeObj->ProcessMessage(&msg));

Exit:
    for (int i = 0; i < _countof(dudes); ++i)
    {
        SafeRelease(dudes[i]);
    }
    SafeRelease(dude);
    SafeRelease(screen);
    return hr;
}
