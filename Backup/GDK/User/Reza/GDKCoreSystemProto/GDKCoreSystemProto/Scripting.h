#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCRIPTING_H_
#define _GDK_SCRIPTING_H_

namespace GDK
{
    enum OpCode
    {
        OpCode_Push = 0,
        OpCode_Add,

        OpCode_Ret, // Must always be last
    };

    struct Instruction
    {
        OpCode Code;
        Var Operand;

        Instruction(OpCode code, const Var& operand)
            : Code(code), Operand(operand) {}
    };

    typedef HRESULT (*NativeScriptMethod)(_In_count_(numArgs) Var* firstArg, _In_ size_t numArgs, _Out_ Var* returnValue);

    struct ScriptMethod
    {
        Instruction*    EntryPoint;
    };

    struct NativeTrampoline
    {
        NativeScriptMethod  EntryPoint;
    };

    enum MethodType
    {
        MethodType_Script = 0,
        MethodType_Native,
    };

    struct ScriptStackFrame
    {
        Var*            StackBase;
        Var*            Locals;
        Instruction*    ip;
    };

    struct MethodStub
    {
        MethodType              Type;
        size_t                  NumParams;

        union
        {
            ScriptMethod        Script;
            NativeTrampoline    Native;
        };
    };

    HRESULT GDK_API CallMethod(_In_ MethodStub* methodStub, _Inout_ Var*& stackPointer);

    struct ExecutionContext
    {
        Var* sp;
        Var* stackBase;
        Var* stackEnd;

        Instruction* ip;
    };

    class ScriptContext
    {
    public:
        static HRESULT Create(_In_ Instruction* ip, _In_ size_t stackSize, _Out_ ScriptContext** context);
        ~ScriptContext();

        HRESULT Step();
        HRESULT Run();

    private:
        ScriptContext();

        ScriptContext(const ScriptContext&);
        ScriptContext& operator= (const ScriptContext&);

        HRESULT StepInternal();

        // String Table
        // Global Variables

        // for now, we only support a single execution context (thread)
        ExecutionContext*   _contexts;
        size_t              _numContexts;
    };

    HRESULT GDK_API InitializeScripting();
    HRESULT GDK_API ExecuteScript(_In_ Instruction* ip, _Inout_ Var*& stackPointer, _In_ const Var* stackEnd);
} // GDK

#endif // _GDK_SCRIPTING_H_
