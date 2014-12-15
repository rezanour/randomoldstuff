#include "StdAfx.h"
#include <map>

#include "Scripting.h"

using namespace GDK;

/*
    Instructions are comprised of a single opcode and a single operand.

    Examples:

    Push        operand     // pushes the operand onto the stack
    Pop         operand     // operand must be param, local, or global. Value on stack is moved to the variable
    Get                     // pops an obj, and then a stringID from the stack. Invokes obj.get(stringID) and stores the result on the stack
    Set                     // pops an obj, a stringID, and then a var from the stack. Invokes obj.set(stringID, var).
    Return      operand     // operand is optional. If missing, value is popped from stack. Stores value in special return location.

    ---------------------------------------------------------------------------------

    Stack space is organized like this:
    -------------------------------------------------
    | | | | | | | | | | | | | | | | | | | | | | | | |
    -------------------------------------------------
    ^           ^ ^     ^
    |           | |     |
    |           | |     --temp space for push/pop and instructions
    |           | |
    |           | --Parameters and local variables pushed in first
    |           |
    |           --Start of frame
    |
    --Base of stack

    When a new frame is created, the stack location it starts at is stored. When the frame 
    is cleaned up, we just unwind the current stack pointer to the start of that frame. This
    means any new Vars pushed onto the stack will overwrite the memory there, which is exactly what
    we want. The Vars aren't immediately cleaned up, but all references to them (in script) should be gone
    once the frame is gone, so we can safely overwrite them later.
*/

struct Frame
{
    Var*        Base;
    byte_t      NumParams;
    uint16_t    NumLocals;
};

typedef HRESULT (*OpHandler)(_In_ const GDK::Var& operand, _Inout_ Var*& stackPointer, _In_ const Var* stackEnd, _In_ const Frame* frame);

static OpHandler g_opHandlers[OpCode_Ret] = { 0 };

HRESULT GDK_API CreateFrame(_Inout_ Var*& stackPointer, _In_ const Var* stackEnd, _In_ byte_t numParams, _In_ uint16_t numLocals, _Out_ Frame* frame)
{
    // validate parameters
    CHECKP(stackPointer, E_INVALIDARG);
    CHECKP(stackEnd, E_INVALIDARG);
    CHECKP(frame, E_POINTER);
    CHECKB(stackPointer + numParams + numLocals < stackEnd, E_OUTOFMEMORY);

    frame->Base = stackPointer;
    frame->NumParams = numParams;
    frame->NumLocals = numLocals;
    for (size_t i = 0, count = numParams + numLocals; i < count; ++i)
    {
        (stackPointer++)->SetType(VarType_Empty);
    }

    return S_OK;
}

HRESULT GDK_API DestroyFrame(_Inout_ Var*& stackPointer, _In_ Frame* frame)
{
    // validate parameters
    CHECKP(stackPointer, E_INVALIDARG);
    CHECKP(frame, E_INVALIDARG);

    // release any object references in our variables
    for (size_t i = 0, count = frame->NumParams + frame->NumLocals; i < count; ++i)
    {
        // cleanup frame->Base[i];
    }

    stackPointer = frame->Base;
    return S_OK;
}


HRESULT PushOpHandler(_In_ const GDK::Var& operand, _Inout_ Var*& stackPointer, _In_ const Var* stackEnd, _In_ const Frame* frame)
{
    CHECKP(stackPointer, E_INVALIDARG);
    CHECKP(stackEnd, E_INVALIDARG);
    CHECKB(stackPointer < stackEnd, E_OUTOFMEMORY);

    switch (operand.GetType())
    {
    case VarType_Param:
        {
            int64_t value = operand.ToInt();
            CHECKB(value < frame->NumParams, E_INVALIDARG);
            *(stackPointer++) = frame->Base[value];
        }
        break;
    case VarType_Local:
        {
            int64_t value = operand.ToInt();
            CHECKB(value < frame->NumLocals, E_INVALIDARG);
            *(stackPointer++) = frame->Base[frame->NumParams + value];
        }
        break;
    default:
        {
            *(stackPointer++) = operand;
        }
        break;
    }
    return S_OK;
}

HRESULT AddOpHandler(_In_ const GDK::Var& operand, _Inout_ Var*& stackPointer, _In_ const Var* stackEnd, _In_ const Frame* frame)
{
    UNREFERENCED_PARAMETER(frame);

    CHECKP(stackPointer, E_INVALIDARG);
    CHECKP(stackEnd, E_INVALIDARG);
    CHECKB(operand.IsEmpty(), E_INVALIDARG);

    Var* a = --stackPointer;
    Var* b = --stackPointer;

    double r1 = a->ToReal();
    double r2 = b->ToReal();
    stackPointer->SetType(VarType_Real);
    (stackPointer++)->SetReal(r1 + r2);

    return S_OK;
}

HRESULT ScriptContext::Create(_In_ Instruction* ip, _In_ size_t stackSize, _Out_ ScriptContext** context)
{
    HRESULT hr = S_OK;

    CHECKP(ip, E_INVALIDARG);
    CHECKB(stackSize > 0, E_INVALIDARG);
    CHECKP(context, E_POINTER);

    *context = new ScriptContext;
    CHECKP(*context, E_OUTOFMEMORY);

    (*context)->_contexts = new ExecutionContext[1];
    if (!(*context)->_contexts)
    {
        delete *context;
        CHECK(E_OUTOFMEMORY);
    }

    (*context)->_numContexts = 1;
    (*context)->_contexts[0].ip = ip;
    (*context)->_contexts[0].stackBase = new Var[stackSize];
    if (!(*context)->_contexts[0].stackBase)
    {
        delete *context;
        CHECK(E_OUTOFMEMORY);
    }

    (*context)->_contexts[0].stackEnd = &(*context)->_contexts[0].stackBase[stackSize - 1];
    (*context)->_contexts[0].sp = (*context)->_contexts[0].stackBase;

    return S_OK;
}

ScriptContext::ScriptContext()
    : _contexts(nullptr), _numContexts(0)
{
}

ScriptContext::~ScriptContext()
{
    if (_contexts)
    {
        delete [] _contexts;
    }
}

HRESULT ScriptContext::Step()
{
    return E_NOTIMPL;
}

HRESULT ScriptContext::Run()
{
    return ExecuteScript(_contexts[0].ip, _contexts[0].sp, _contexts[0].stackEnd);
}

HRESULT ScriptContext::StepInternal()
{
    return E_NOTIMPL;
}

HRESULT GDK_API GDK::InitializeScripting()
{
    CHECKP(!g_opHandlers[0], HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED));

    g_opHandlers[OpCode_Push] = PushOpHandler;
    g_opHandlers[OpCode_Add] = AddOpHandler;

    return S_OK;
}

HRESULT GDK_API GDK::ExecuteScript(_In_ Instruction* ip, _Inout_ Var*& stackPointer, _In_ const Var* stackEnd)
{
    CHECKP(g_opHandlers[0], E_UNEXPECTED);

    HRESULT hr = S_OK;

    Frame frame;
    CHECK(CreateFrame(stackPointer, stackEnd, 0, 0, &frame));

    while (ip->Code != OpCode_Ret)
    {
        CHECK(g_opHandlers[ip->Code](ip->Operand, stackPointer, stackEnd, &frame));
        ++ip;
    }

    CHECK(DestroyFrame(stackPointer, &frame));
    return S_OK;
}

HRESULT GDK_API GDK::CallMethod(_In_ MethodStub* methodStub, _Inout_ Var*& stackPointer)
{
    HRESULT hr = S_OK;

    CHECKP(methodStub, E_INVALIDARG);
    CHECKP(stackPointer, E_INVALIDARG);

    if (methodStub->Type == MethodType_Script)
    {
        // point locals back as many params as needed
        ScriptStackFrame frame;
        frame.Locals = (stackPointer - methodStub->NumParams);
        frame.StackBase = stackPointer;
        frame.ip = methodStub->Script.EntryPoint;

        // Execute frame
    }
    else if (methodStub->Type == MethodType_Native)
    {
        Var returnValue;
        CHECK(methodStub->Native.EntryPoint(stackPointer - methodStub->NumParams, methodStub->NumParams, &returnValue));
        // push return value onto stack
    }
    else
    {
        assert(false && "Unknown method type");
    }

    return S_OK;
}

enum OpCodes
{
    Op_Nop = 0,
    Op_Push,
    Op_Add,
    Op_Ret
};

struct Inst
{
    OpCodes code : 8;
    byte_t data1;
    uint32_t data2;

    Inst(OpCodes op, byte_t d1, uint32_t d2)
        : code(op), data1(d1), data2(d2)
    {
    }
};

struct ScriptFrame
{
    uint16_t frameBase;
    uint16_t returnAddress;
};

struct MicroThreadContext
{
    byte_t* stack;
    ScriptFrame* callStack;
    Inst* code;

    // offsets
    uint16_t sp;    // stack pointer
    uint16_t fp;    // frame pointer
    uint16_t ip;    // instruction pointer

    // extents
    uint16_t spMax; // end of stack
    uint16_t fpMax; // end of callstack
    uint16_t ipMax; // end of code

    MicroThreadContext()
        : stack(nullptr), callStack(nullptr), code(nullptr),
          sp(0), fp(0), ip(0), spMax(0), fpMax(0), ipMax(0)
    {
    }

    ~MicroThreadContext()
    {
        if (stack)
        {
            delete [] stack;
        }
        if (callStack)
        {
            delete [] callStack;
        }
    }
};

HRESULT GDK_API CallOperation(_In_ MicroThreadContext* thread, _In_ uint16_t methodOffset)
{
    CHECKB(thread->fp < thread->fpMax, E_OUTOFMEMORY);
    CHECKB(methodOffset < thread->ipMax, HRESULT_FROM_WIN32(ERROR_INVALID_INDEX));

    uint16_t fp = thread->fp;
    thread->callStack[fp].frameBase = thread->sp;
    thread->callStack[fp].returnAddress = thread->ip;
    ++thread->fp;

    thread->ip = methodOffset;
    return S_OK;
}

HRESULT GDK_API SampleScriptMethod(_In_ MicroThreadContext* thread)
{
    /*
    signature is:

    int foo(int a, int b)
    {
        return a + b;
    }

    byte code is:

    push base: -(2*sizeof(int)) // find parameter at frameBase - 2 integers back (a)
    push base: -sizeof(int)     // find parameter at frameBase - 1 integer back (b)
    add                         // pop those two off the stack and add them, push the result back
    ret int                     // pop an int off the stack, unwind the frame, and then push the int back in as our return value

    */

    // optimized out the push/pop of intermediates
    uint16_t thisFrame = thread->fp - 1;
    byte_t* frameBase = &thread->stack[thread->callStack[thisFrame].frameBase];
    int32_t a = *(int32_t*)(frameBase - 2 * sizeof(int32_t));
    int32_t b = *(int32_t*)(frameBase - sizeof(int32_t));

    // unwind and push in return value
    thread->ip = thread->callStack[thisFrame].returnAddress;
    *(int32_t*)(frameBase - 2 * sizeof(int32_t)) = a + b;
    thread->sp = thread->callStack[thisFrame].frameBase - 2 * sizeof(int32_t);
    --(thread->fp);
    return S_OK;
}

HRESULT GDK_API ExecuteMicroThread(_In_ MicroThreadContext* context);
HRESULT GDK_API InitOpHandlers();

HRESULT GDK_API Test123()
{
    InitOpHandlers();

    MicroThreadContext thread;
    thread.spMax = 256;
    thread.stack = new byte_t[thread.spMax];
    CHECKP(thread.stack, E_OUTOFMEMORY);

    thread.fpMax = 100;
    thread.callStack = new ScriptFrame[thread.fpMax];
    CHECKP(thread.callStack, E_OUTOFMEMORY);

    float a = 123.456f;
    float b = 876.544f;
    Inst simpleProgram[] = 
    {
        Inst(Op_Push, 2, *(uint32_t*)&a),
        Inst(Op_Push, 2, *(uint32_t*)&b),
        Inst(Op_Add, 2, 0),
    };

    thread.ipMax = _countof(simpleProgram);
    thread.code = simpleProgram;

    ExecuteMicroThread(&thread);

    return S_OK;
}

typedef HRESULT (*OpCodeHandler)(_In_ const Inst& inst, _In_ MicroThreadContext* context);

static OpCodeHandler g_OpHandlers[Op_Ret];

HRESULT GDK_API ExecuteMicroThread(_In_ MicroThreadContext* context)
{
    assert(context);

    HRESULT hr = S_OK;

    while (context->ip < context->ipMax)
    {
        const Inst& inst = context->code[context->ip];
        if (g_OpHandlers[inst.code])
        {
            CHECK(g_OpHandlers[inst.code](inst, context));
        }
        ++context->ip;
    }

    return S_OK;
}

HRESULT PushHandler(_In_ const Inst& inst, _In_ MicroThreadContext* context)
{
    switch (inst.data1)
    {
    case 0: // bool
        *(bool*)&context->stack[context->sp] = *(bool*)&inst.data2;
        context->sp += sizeof(bool);
        break;

    case 1: // int
        *(int32_t*)&context->stack[context->sp] = *(int32_t*)&inst.data2;
        context->sp += sizeof(int32_t);
        break;

    case 2: // float
        *(float*)&context->stack[context->sp] = *(float*)&inst.data2;
        context->sp += sizeof(float);
        break;

    default:
        assert(false);
    }

    return S_OK;
}

HRESULT AddHandler(_In_ const Inst& inst, _In_ MicroThreadContext* context)
{
    switch (inst.data1)
    {
    case 1: // int
        {
            context->sp -= sizeof(int32_t);
            int32_t b = *(int32_t*)&context->stack[context->sp];
            context->sp -= sizeof(int32_t);
            int32_t a = *(int32_t*)&context->stack[context->sp];
            int32_t sum = a + b;
            *(int32_t*)&context->stack[context->sp] = *(int32_t*)&sum;
            context->sp += sizeof(int32_t);
        }
        break;

    case 2: // float
        {
            context->sp -= sizeof(float);
            float b = *(float*)&context->stack[context->sp];
            context->sp -= sizeof(float);
            float a = *(float*)&context->stack[context->sp];
            float sum = a + b;
            *(float*)&context->stack[context->sp] = *(float*)&sum;
            context->sp += sizeof(float);
        }
        break;

    default:
        assert(false);
    }

    return S_OK;
}

HRESULT GDK_API InitOpHandlers()
{
    g_OpHandlers[Op_Push] = PushHandler;
    g_OpHandlers[Op_Add] = AddHandler;
    return S_OK;
}
