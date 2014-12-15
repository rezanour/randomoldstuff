#include <CoreServices\Configuration.h>
#include "SubTest.h"
#include <GDK\Transform.h>

#define DIRECTXMATH_INTEROP
#include <L3DMath\L3DMath.h>

using namespace CoreServices;

#include <iostream>
using namespace std;

struct Foo
{
    int type;
    float poo;
};

istream& operator>> (istream& stream, Foo& foo)
{
    char c;
    stream >> foo.type >> c >> foo.poo;
    return stream;
}

namespace
{
    bool CreateNewTest();
    bool SetGetValueTest();
    bool GetNonExistingValueTest();
    bool LoadSaveTest();
    bool LoadMalformedTest();
    bool StreamTest();
    bool TransformTest();
    bool XMFLOATTest();

    SubTest subTests[] =
    {
        DefineSubTest(CreateNewTest),
        DefineSubTest(SetGetValueTest),
        DefineSubTest(GetNonExistingValueTest),
        DefineSubTest(LoadSaveTest),
        DefineSubTest(LoadMalformedTest),
        DefineSubTest(StreamTest),
        DefineSubTest(TransformTest),
        DefineSubTest(XMFLOATTest),
    };

    bool CreateNewTest()
    {
        ConfigurationPtr spConfiguration;

        // Create a blank configuration object
        HRESULT hr = Configuration::Create(&spConfiguration);

        // Test test passes if we get a valid (non null) configuration object back
        return (SUCCEEDED(hr) && spConfiguration);
    }

    bool SetGetValueTest()
    {
        ConfigurationPtr spConfiguration;

        // Create a blank configuration object
        HRESULT hr = Configuration::Create(&spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        // Set a couple values
        spConfiguration->SetValue("Awesome", "Sauce");
        spConfiguration->SetValue("Dookie", "Smells");

        // Read them back to ensure they were set correctly
        if (spConfiguration->GetValue<std::string>("Awesome", "") != "Sauce")
            return false;

        if (spConfiguration->GetStringValue("Dookie") != "Smells")
            return false;

        // If we got here, we're golden
        return true;
    }

    bool GetNonExistingValueTest()
    {
        ConfigurationPtr spConfiguration;

        // Create a blank configuration object
        HRESULT hr = Configuration::Create(&spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        // Try and read a value that doesn't exist
        if (spConfiguration->GetStringValue("Awesome") != "")
            return false;

        // If we got here, we're golden
        return true;
    }

    bool LoadSaveTest()
    {
        ConfigurationPtr spConfiguration;

        // Load a test configuration file
        HRESULT hr = Configuration::Load(L"ConfigurationTests.ini", &spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        if (spConfiguration->GetStringValue("Awesome") != std::string("Sauce"))
            return false;

        if (spConfiguration->GetStringValue("Dookie") != "Smells")
            return false;

        // Save it out to a new file
        hr = spConfiguration->Save(L"foo.ini");
        if (FAILED(hr))
            return false;

        // Read back in the new file and then test it for the same values
        hr = Configuration::Load(L"foo.ini", &spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        if (spConfiguration->GetStringValue("Awesome") != "Sauce")
            return false;

        if (spConfiguration->GetStringValue("Dookie") != "Smells")
            return false;

        return true;
    }

    bool LoadMalformedTest()
    {
        ConfigurationPtr spConfiguration;

        // Load a test configuration file
        HRESULT hr = Configuration::Load(L"Malformed.ini", &spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        // Malformed lines should just be skipped, so we should only have our good Poo property
        if (spConfiguration->GetCount() != 1)
            return false;

        if (spConfiguration->GetStringValue("Poo") != "Foo")
            return false;

        return true;
    }

    bool StreamTest()
    {
        ConfigurationPtr spConfiguration;

        HRESULT hr = Configuration::Load(L"StreamTest.ini", &spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        Foo foo;
        foo = spConfiguration->GetValue<Foo>("Foo", Foo());

        if (foo.type != 2343)
            return false;

        if (foo.poo != 343.30438f)
            return false;

        return true;
    }

    bool TransformTest()
    {
        ConfigurationPtr spConfiguration;

        HRESULT hr = Configuration::Create(&spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        GDK::Transform transform;
        transform.Position = DirectX::XMFLOAT3(1.0f, 2.5f, 3.0f);
        transform.Orientation = DirectX::XMFLOAT4(4.0f, 5.5f, 6.0f, 7.0f);
        transform.Scale = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);

        spConfiguration->SetValue("Transform", transform);

        GDK::Transform defaultTransform;
        GDK::Transform returnedTransform;

        returnedTransform = spConfiguration->GetValue("Transform", defaultTransform);

        if (transform.Position.x != returnedTransform.Position.x ||
            transform.Position.y != returnedTransform.Position.y || 
            transform.Position.z != returnedTransform.Position.z || 
            transform.Orientation.x != returnedTransform.Orientation.x ||
            transform.Orientation.y != returnedTransform.Orientation.y || 
            transform.Orientation.z != returnedTransform.Orientation.z || 
            transform.Orientation.w != returnedTransform.Orientation.w || 
            transform.Scale.x != returnedTransform.Scale.x ||
            transform.Scale.y != returnedTransform.Scale.y ||
            transform.Scale.z != returnedTransform.Scale.z)
        {
            return false;
        }

        return true;
    }

    bool XMFLOATTest()
    {
        ConfigurationPtr spConfiguration;

        HRESULT hr = Configuration::Create(&spConfiguration);
        if (FAILED(hr) || !spConfiguration)
            return false;

        GDK::Transform transform;
        transform.Position = DirectX::XMFLOAT3(1.0f, 2.5f, 3.0f);
        transform.Orientation = DirectX::XMFLOAT4(4.0f, 5.5f, 6.0f, 7.0f);
        transform.Scale = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);

        spConfiguration->SetValue("Transform.Position", transform.Position);
        spConfiguration->SetValue("Transform.Orientation", transform.Orientation);
        spConfiguration->SetValue("Transform.Scale", transform.Scale);

        GDK::Transform defaultTransform;
        GDK::Transform returnedTransform;

        returnedTransform.Position = spConfiguration->GetValue("Transform.Position", defaultTransform.Position);
        returnedTransform.Orientation = spConfiguration->GetValue("Transform.Orientation", defaultTransform.Orientation);
        returnedTransform.Scale = spConfiguration->GetValue("Transform.Scale", defaultTransform.Scale);

        if (transform.Position.x != returnedTransform.Position.x ||
            transform.Position.y != returnedTransform.Position.y || 
            transform.Position.z != returnedTransform.Position.z || 
            transform.Orientation.x != returnedTransform.Orientation.x ||
            transform.Orientation.y != returnedTransform.Orientation.y || 
            transform.Orientation.z != returnedTransform.Orientation.z || 
            transform.Orientation.w != returnedTransform.Orientation.w || 
            transform.Scale.x != returnedTransform.Scale.x ||
            transform.Scale.y != returnedTransform.Scale.y ||
            transform.Scale.z != returnedTransform.Scale.z)
        {
            return false;
        }

        return true;
    }
}

bool ConfigurationTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}


