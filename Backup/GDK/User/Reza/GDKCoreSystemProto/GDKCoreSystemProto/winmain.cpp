#include "StdAfx.h"
#include "Process.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "ObjectManager.h"
#include "Message.h"
#include "Scripting.h"

HRESULT GDK_API ScriptTest();
extern HRESULT GDK_API Test123();

void MyTracer(_In_ byte_t channel, _In_ const wchar_t* message)
{
    UNREFERENCED_PARAMETER(channel);
    UNREFERENCED_PARAMETER(message);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    using namespace GDK;

    HRESULT hr = S_OK;

    FATAL(InitializeDebugLogListener());
    Configuration::Load(L"configuration.ini");
    SetLogFile(Configuration::GetString(StringID::GetStringID(L"logFile"), L"test123.txt"));

    LogF(L"key1 = %s\n", Configuration::GetString(StringID::GetStringID(L"key1")));

    if (Configuration::GetBool(StringID::GetStringID(L"debugEnabled"), false))
    {
        Log(L"Debugging enabled\n");
    }

    LogF(L"maxGoozes = %d\n", Configuration::GetInt(StringID::GetStringID(L"maxGoozes"), 0));

    FATAL(AddDebugListener(MyTracer));

    StringID name = StringID::RegisterString(L"Dude");
    if (name)
    {
        wchar_t temp[25] = {0};
        for (size_t i = 0; i < 10000; ++i)
        {
            wsprintf(temp, L"test%d", i);
            StringID::RegisterString(temp);
        }

        StringID other(name);

        StringID test(StringID::GetStringID(L"test1939"));
        DebugStringF(L"%s\n", StringID::GetString(test));

        if (other == name)
        {
            DebugStringF(L"same! %s & %s\n", StringID::GetString(other), StringID::GetString(name));
        }
    }

    FATAL(Executive::Startup());

    Executive* exec = Executive::GetExecutive();
    FATALP(exec, E_FAIL);

    Log(L"Successfully started executive\n");

    FATAL(RemoveDebugListener(MyTracer));

    void* p = exec->GetMemoryManager()->Alloc(128, "startup test mem");
    FATALP(p, E_POINTER);

    Message* msg = nullptr;
    FATAL(Message::Create(1234, &msg));

    LogF(3, L"Message: %d, %d\n", msg->GetID(), msg->GetType());

    delete msg;

    exec->GetMemoryManager()->Free(p);
    Executive::Shutdown();

    FATAL(ScriptTest());

    FATAL(Test123());

    return 0;
}

HRESULT GDK_API ScriptTest()
{
    using namespace GDK;

    HRESULT hr = S_OK;

    CHECK(InitializeScripting());

    Instruction code[] = 
    {
        Instruction(OpCode_Push, Var(35.0)),
        Instruction(OpCode_Push, Var(52.0)),
        Instruction(OpCode_Add, Var()),
        Instruction(OpCode_Ret, Var()),
    };

    ScriptContext* context = nullptr;
    CHECK(ScriptContext::Create(code, 10, &context));
    CHECK(context->Run());

    Var stack[100];
    Var* sp = stack;
    Var* end = &stack[_countof(stack)-1];

    CHECK(ExecuteScript(code, sp, end));

    return S_OK;
}
