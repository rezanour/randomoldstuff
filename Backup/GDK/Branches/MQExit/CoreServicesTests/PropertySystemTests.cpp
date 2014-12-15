#include "StdAfx.h"
#include "SubTest.h"

using namespace CoreServices;
using namespace GDK;

namespace
{
    bool CreatePropertyTest();
    bool DeletePropertyTest();
    bool GetPropertyTest();
    bool ErrorCaseTests();

    static SubTest subTests[] =
    {
        // Run the test twice as it found a couple issues that way
        DefineSubTest(CreatePropertyTest),
        DefineSubTest(DeletePropertyTest),
        DefineSubTest(GetPropertyTest),
        DefineSubTest(ErrorCaseTests),
    };

    bool CreatePropertyTest()
    {
        stde::ref_counted_ptr<PropertyBag> props(new PropertyBag());

        stde::ref_counted_ptr<IProperty> spProp;
        if (FAILED(props->CreateProperty(PropertyType::Integer, "Health", &spProp)))
            return false;

        if (std::string("Health") != spProp->GetName())
            return false;

        if (spProp->GetType() != PropertyType::Integer)
            return false;

        if (FAILED(spProp->SetInt(100)))
            return false;

        int val = 0;
        if (FAILED(spProp->AsInt(&val)))
            return false;

        if (val != 100)
            return false;

        return true;
    }

    bool DeletePropertyTest()
    {
        stde::ref_counted_ptr<PropertyBag> props(new PropertyBag());

        stde::ref_counted_ptr<IProperty> spProp;
        if (FAILED(props->CreateProperty(PropertyType::Float, "Health", &spProp)))
            return false;

        if (props->GetCount() != 1)
            return false;

        if (FAILED(props->DeleteProperty("Health")))
            return false;

        if (props->GetCount() != 0)
            return false;

        return true;
    }

    bool GetPropertyTest()
    {
        stde::ref_counted_ptr<PropertyBag> props(new PropertyBag());

        stde::ref_counted_ptr<IProperty> spProp;
        if (FAILED(props->CreateProperty(PropertyType::Boolean, "Alive", &spProp)))
            return false;

        if (FAILED(spProp->SetBool(true)))
            return false;

        spProp = nullptr;
        if (FAILED(props->CreateProperty(PropertyType::String, "Guild", &spProp)))
            return false;

        if (FAILED(spProp->SetString("The Awesome Jawesomes")))
            return false;

        if (props->GetCount() != 2)
            return false;

        // verify

        spProp = nullptr;
        if (FAILED(props->GetProperty("Alive", &spProp)))
            return false;

        bool alive = false;
        if (FAILED(spProp->AsBool(&alive)))
            return false;

        spProp = nullptr;
        if (FAILED(props->GetProperty("Guild", &spProp)))
            return false;

        char guild[100] = {0};
        if (FAILED(spProp->AsString(guild, _countof(guild))))
            return false;

        if (!alive)
            return false;

        if (std::string("The Awesome Jawesomes") != guild)
            return false;

        return true;
    }

    bool ErrorCaseTests()
    {
        stde::ref_counted_ptr<PropertyBag> props(new PropertyBag());

        stde::ref_counted_ptr<IProperty> spProp;
        if (FAILED(props->CreateProperty(PropertyType::Boolean, "Alive", &spProp)))
            return false;

        int i = 0;
        if (spProp->AsInt(&i) != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
            return false;

        spProp = nullptr;
        if (FAILED(props->CreateProperty(PropertyType::String, "Guild", &spProp)))
            return false;

        if (spProp->SetFloat(123.0f) != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
            return false;

        spProp = nullptr;
        if (props->GetProperty("Alive123", &spProp) != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            return false;

        if (props->DeleteProperty("Something That doesn't exist") != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            return false;

        spProp = nullptr;
        if (props->CreateProperty(PropertyType::Integer, nullptr, &spProp) != E_INVALIDARG)
            return false;

        spProp = nullptr;
        if (props->CreateProperty(PropertyType::Integer, "", nullptr) != E_POINTER)
            return false;

        spProp = nullptr;
        if (props->GetProperty(nullptr, &spProp) != E_INVALIDARG)
            return false;

        spProp = nullptr;
        if (props->GetProperty("", nullptr) != E_POINTER)
            return false;

        spProp = nullptr;
        if (props->GetProperty(32, &spProp) != E_INVALIDARG)
            return false;

        spProp = nullptr;
        if (props->DeleteProperty(nullptr) != E_INVALIDARG)
            return false;

        return true;
    }
}

bool PropertySystemTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}

