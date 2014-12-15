#include "StdAfx.h"

#ifdef BUILD_TESTS

#include "Testing.h"
#include "ObjectModel\GameObject.h"

#include <GDK\RefCounted.h>
#include <stde\non_copyable.h>

using namespace GDK;
using namespace DirectX;

namespace GDK
{
    bool BasicTest();
    bool TransformTest();

    static SubTest subTests[] =
    {
        // Run the test twice as it found a couple issues that way
        DefineSubTest(BasicTest),
        DefineSubTest(TransformTest),
    };

    bool BasicTest()
    {
        GameObjectPtr spGameObject;

        if (FAILED(GameObject::Create("dude", spGameObject)))
            return false;

        if (std::string("dude") != spGameObject->GetName())
            return false;

        std::string newName("awesome dude");

        spGameObject->SetName(newName.c_str());
        if (newName != spGameObject->GetName())
            return false;

        stde::ref_counted_ptr<GDK::IPropertyBag> spProperties;
        if (FAILED(spGameObject->GetPropertyBag(&spProperties)))
            return false;

        stde::ref_counted_ptr<IProperty> spHealth;
        stde::ref_counted_ptr<IProperty> spPoisened;

        if (FAILED(spProperties->CreateProperty(PropertyType::Integer, "Health", &spHealth)))
            return false;

        if (FAILED(spHealth->SetInt(100)))
            return false;

        if (FAILED(spProperties->CreateProperty(PropertyType::Boolean, "Poisened", &spPoisened)))
            return false;

        if (FAILED(spPoisened->SetBool(true)))
            return false;

        // Here's how games (script) use the properties
        bool poisened = false;
        int health = 0;

        // Query state
        if (FAILED(spPoisened->AsBool(&poisened)))
            return false;

        if (FAILED(spHealth->AsInt(&health)))
            return false;

        // Perform game logic
        if (poisened)
            health -= 10;

        // Update state
        if (FAILED(spHealth->SetInt(health)))
            return false;

        return true;
    }

    bool TransformTest()
    {
        stde::ref_counted_ptr<GameObject> spGameObject;
        Transform transform;
        size_t salt = 0;

        if (FAILED(GameObject::Create("dude", spGameObject)))
            return false;

        spGameObject->GetTransform(&transform);

        Rotate(transform, XMVectorSet(0, 1, 0, 0), 12);

        salt = spGameObject->GetTransformSalt();
        spGameObject->SetTransform(transform);

        if (salt == spGameObject->GetTransformSalt())
            return false;

        spGameObject->GetTransform(&transform);
        if (transform.Orientation.y == 0)
            return false;

        return true;
    }
}

using namespace GDK;

bool GameObjectTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}

#endif

