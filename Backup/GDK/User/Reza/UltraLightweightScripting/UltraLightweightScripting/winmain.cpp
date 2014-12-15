#include "StdAfx.h"
#include "Object.h"
#include "Variable.h"
#include "Method.h"

#include "TypeInfo.h"
#include "ObjectInstance.h"

extern HRESULT DudeTakeDamage(_In_opt_ GDK::Object* instance, _In_ size_t numParams, _In_ GDK::Variable** parameters, _Out_opt_ GDK::Variable* returnValue);

HRESULT DudeTakeDamage(_In_opt_ GDK::ObjectInstance* instance, _In_ GDK::Var* damage, _Out_opt_ GDK::Var* returnValue);
HRESULT AddThreeNumbers(_In_opt_ GDK::ObjectInstance* instance, _In_ GDK::Var* a, _In_ GDK::Var* b, _In_ GDK::Var* c, _Out_opt_ GDK::Var* returnValue);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    using namespace GDK;

    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    Variable var(123.456);
    Variable var2(77);

    Variable var3(var.AsReal() + var2.AsReal());

    if (abs(var3.AsReal() - 200.456) < 0.0001)
    {
        OutputDebugString(L"Works like a champ!\n");
    }

    size_t count = 100000;


    Object* dude;
    HRESULT hr = Object::Create(L"Dude", &dude);
    if (SUCCEEDED(hr))
    {
        auto health = dude->GetProperty(L"Health");
        if (health)
        {
            health->SetValue(health->AsReal() - 10.0f);
        }

        std::wstring methodName(L"TakeDamage");
        Variable amount(134.22);
        Variable* args[] = { &amount };

        ULONGLONG start = GetTickCount64();

        for (size_t i = 0; i < count; ++i)
        {
            dude->InvokeMethod(methodName, 1, args, nullptr);
        }

        ULONGLONG end = GetTickCount64();
        ULONGLONG scriptTime = end - start;

        start = GetTickCount64();

        for (size_t i = 0; i < count; ++i)
        {
            DudeTakeDamage(dude, 1, args, nullptr);
        }

        end = GetTickCount64();
        ULONGLONG nativeTime = end - start;

        char message[1000] = {0};
        StringCchPrintfA(message, _countof(message), "ScriptTime: %lu\n", scriptTime);
        OutputDebugStringA(message);
        StringCchPrintfA(message, _countof(message), "NativeTime: %lu\n", nativeTime);
        OutputDebugStringA(message);

        if (health->AsReal() < 89)
        {
            OutputDebugString(L"Method actually worked!\n");
        }

        delete dude;
    }

    TypeInfo dudeType("Dude");

    Method dudeTakeDamage(VariableType_Empty, DudeTakeDamage);
    Method addThreeNumbers(VariableType_Integer, AddThreeNumbers);

    dudeType.AddProperty("Name", VarType_StringID);
    dudeType.AddProperty("Health", VarType_Integer);
    dudeType.AddMethod("TakeDamage", &dudeTakeDamage);
    dudeType.AddMethod("AddThreeNumbers", &addThreeNumbers);

    ObjectInstance* dude2;
    hr = ObjectInstance::CreateFromTypeInfo(&dudeType, &dude2);
    if (SUCCEEDED(hr))
    {
        auto name = dude2->GetProperty(0);
        auto health = dude2->GetProperty(1);

        Var v;
        v.Set(123);
        v.Set(true);

        name->SetString(32);
        health->Set(94.3);

        Var* args[] = { &v };
        hr = dude2->InvokeMethod(0, 1, args, nullptr);
        if (FAILED(hr))
        {
            OutputDebugString(L"method 1 failed\n");
        }

        ULONGLONG start = GetTickCount64();

        for (size_t i = 0; i < count; ++i)
        {
            dude2->InvokeMethod(0, 1, args, nullptr);
        }

        ULONGLONG end = GetTickCount64();
        ULONGLONG scriptTime = end - start;

        char message[1000] = {0};
        StringCchPrintfA(message, _countof(message), "Super ScriptTime: %lu\n", scriptTime);
        OutputDebugStringA(message);

        Var a;
        a.Set(2343.34);
        Var b;
        b.Set(32);
        Var c;
        c.Set(-200);

        Var* args2[] = { &a, &b, &c };
        hr = dude2->InvokeMethod(1, 3, args2, &v);
        if (FAILED(hr))
        {
            OutputDebugString(L"method 2 failed\n");
        }

        if (name && health &&
            name->GetType() == VarType_StringID &&
            health->GetType() == VarType_Integer &&
            name->GetString() == 32 &&
            health->GetInteger() == 94)
        {
            OutputDebugString(L"properties work\n");
        }

        dude2->Release();
    }

    return 0;
}

HRESULT DudeTakeDamage(_In_opt_ GDK::ObjectInstance* instance, _In_ GDK::Var* damage, _Out_opt_ GDK::Var* returnValue)
{
    UNREFERENCED_PARAMETER(returnValue);

    if (!instance || !damage || !damage->IsNumber())
        return E_INVALIDARG;

    auto health = instance->GetProperty(1);
    if (!health)
        return E_UNEXPECTED;

    health->Set(health->GetReal() - damage->GetReal());
    return S_OK;
}

HRESULT AddThreeNumbers(_In_opt_ GDK::ObjectInstance* instance, _In_ GDK::Var* a, _In_ GDK::Var* b, _In_ GDK::Var* c, _Out_opt_ GDK::Var* returnValue)
{
    UNREFERENCED_PARAMETER(instance);

    if (!a || !b || !c || !returnValue)
        return E_INVALIDARG;

    returnValue->Set(a->GetReal() + b->GetReal() + c->GetReal());
    return S_OK;
}
