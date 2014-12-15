#include "StdAfx.h"
#include "Object.h"
#include "Value.h"
#include "Variable.h"
#include "MethodInfo.h"
#include "Property.h"

HRESULT MyAwesomeScriptableMethod(_In_ size_t numParameters, _In_opt_count_(numParameters) GDK::Value* parameters, _Out_opt_ GDK::Value* returnValue);

__declspec(noinline) double HowManyGoozesNative(double myGoozes, double kidsGoozes)
{
    return myGoozes + kidsGoozes;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    using namespace GDK;

    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    Object<>* o = nullptr;
    HRESULT hr = Object<>::Create(&o);
    if (SUCCEEDED(hr))
    {
        uint32_t id = o->GetID();
        IType* type = o->GetType();
        const char* name = o->GetName();

        type;

        char message[1000] = {0};
        hr = StringCchPrintfA(message, _countof(message), "ID: %d, Type: %s, Name: %s\n", 
            id, 
            type->GetName(),
            (name ? name : "none"));

        if (SUCCEEDED(hr))
        {
            OutputDebugStringA(message);
        }
    }

    Value v(32);
    Value v2(64.0f);
    Value v3("foonanny");
    Value v4(L"something");

    Value v5(true);
    Value v6(false);

    v = v2;
    v2.Clear();
    v3.ClearValue();

    if (v2.IsEmpty())
    {
        OutputDebugString(L"Empty!\n");
    }

    Variable a("a", 32);
    Variable b("b", 43.0f);
    Variable c("c", true);
    Variable d("d", "whoa");

    if (a.SetValue(33.0))
    {
        OutputDebugString(L"Converted\n");
    }

    if (c.CopyValueTo(d))
    {
        OutputDebugString(L"Shouldn't work\n");
    }

    if (!a.CopyValueTo(b))
    {
        OutputDebugString(L"Should have worked\n");
    }

    MethodInfo* method;
    ValueType parameterTypes[] = { ValueType_Number, ValueType_Number };
    const char* parameterNames[] = { "myGoozes", "kidsGoozes" };
    hr = MethodInfo::Create(MethodFlag_Static, ValueType_Number, "HowManyGoozes", 2, parameterTypes, parameterNames, MyAwesomeScriptableMethod, &method);
    if (SUCCEEDED(hr))
    {
        OutputDebugString(L"Invoking Method\n");

        size_t count = 1000000;

        Value args[] = { 124, 53 };
        Value returnValue;
        size_t successes = 0;
        ULONGLONG start = GetTickCount64();
        for (size_t i = 0; i < count; ++i)
        {
            hr = method->InvokeMethod(_countof(args), args, &returnValue);
            if (SUCCEEDED(hr))
            {
                double result;
                if (returnValue.AsNumber(&result) && result == 177)
                {
                    successes++;
                }
            }
        }
        ULONGLONG end = GetTickCount64();
        ULONGLONG scriptTime = end - start;

        size_t nativeSuccesses = 0;
        start = GetTickCount64();
        for (size_t i = 0; i < count; ++i)
        {
            hr = MyAwesomeScriptableMethod(_countof(args), args, &returnValue);
            if (SUCCEEDED(hr))
            {
                double result;
                if (returnValue.AsNumber(&result) && result == 177)
                {
                    nativeSuccesses++;
                }
            }
        }
        end = GetTickCount64();
        ULONGLONG nativeTime = end - start;

        start = GetTickCount64();
        for (size_t i = 0; i < count; ++i)
        {
            double result = HowManyGoozesNative(124, 53);
            if (result == 177)
            {
                nativeSuccesses++;
            }
        }
        end = GetTickCount64();
        ULONGLONG rawNativeTime = end - start;

        if (successes > 0)
        {
            OutputDebugString(L"Really? this shit actually works!?!?!\n");

            char message[1000] = {0};
            StringCchPrintfA(message, _countof(message), "ScriptTime: %lu\n", scriptTime);
            OutputDebugStringA(message);
            StringCchPrintfA(message, _countof(message), "NativeTime: %lu\n", nativeTime);
            OutputDebugStringA(message);
            StringCchPrintfA(message, _countof(message), "RawNativeTime: %lu\n", rawNativeTime);
            OutputDebugStringA(message);
        }
    }

    if (SUCCEEDED(hr))
    {
        Property dudeHealth(ValueType_Number, "Dude::Health");
        Value health(100);
        dudeHealth.SetValue(health);

        health.Set(0);
        double healthD;
        if (dudeHealth.GetValue(&health) && health.AsNumber(&healthD) && healthD == 100)
        {
            OutputDebugString(L"Health Property works\n");
        }
    }

    return 0;
}

// enter into script as:
// static int HowManyGoozes(int myGoozes, int kidsGoozes)
// { return myGoozes + kidsGoozes; }
// 
HRESULT MyAwesomeScriptableMethod(_In_ size_t numParameters, _In_opt_count_(numParameters) GDK::Value* parameters, _Out_opt_ GDK::Value* returnValue)
{
    // Validate
    if (numParameters != 2)
        return E_INVALIDARG;

    if (!parameters)
        return E_INVALIDARG;

    double myGoozes;
    if (!parameters[0].AsNumber(&myGoozes))
    {
        return E_INVALIDARG;
    }

    double kidsGoozes;
    if (!parameters[1].AsNumber(&kidsGoozes))
    {
        return E_INVALIDARG;
    }

    // don't do anything if they don't want the result
    if (!returnValue)
        return S_OK;

    returnValue->Set(myGoozes + kidsGoozes);
    //*returnValue = myGoozes + kidsGoozes;
    return S_OK;
}

