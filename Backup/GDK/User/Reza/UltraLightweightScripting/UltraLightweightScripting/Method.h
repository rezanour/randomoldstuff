#pragma once

#include "Variable.h"

namespace GDK
{
    typedef HRESULT (*ScriptMethod)(_In_opt_ Object* instance, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue);

    class Var;
    class ObjectInstance;
    typedef HRESULT (*ScriptNoArguments)(_In_opt_ ObjectInstance* instance, _Out_opt_ Var* returnValue);
    typedef HRESULT (*ScriptOneArgument)(_In_opt_ ObjectInstance* instance, _In_ Var* arg0, _Out_opt_ Var* returnValue);
    typedef HRESULT (*ScriptTwoArguments)(_In_opt_ ObjectInstance* instance, _In_ Var* arg0, _In_ Var* arg1, _Out_opt_ Var* returnValue);
    typedef HRESULT (*ScriptThreeArguments)(_In_opt_ ObjectInstance* instance, _In_ Var* arg0, _In_ Var* arg1, _In_ Var* arg2, _Out_opt_ Var* returnValue);
    typedef HRESULT (*ScriptFourArguments)(_In_opt_ ObjectInstance* instance, _In_ Var* arg0, _In_ Var* arg1, _In_ Var* arg2, _In_ Var* arg3, _Out_opt_ Var* returnValue);
    typedef HRESULT (*ScriptMultiArguments)(_In_opt_ ObjectInstance* instance, _In_ size_t numArgs, _In_count_(numArgs) Var** args, _Out_opt_ Var* returnValue);

    class Method
    {
    public:
        Method(_In_ VariableType returnType, _In_ size_t numParams, _In_ VariableType* parameterTypes, _In_ ScriptMethod entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptNoArguments entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptOneArgument entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptTwoArguments entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptThreeArguments entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptFourArguments entryPoint);
        Method(_In_ VariableType returnType, _In_ ScriptMultiArguments entryPoint, _In_ size_t numArgs);
        ~Method();

        // Accessors
        VariableType GetReturnType() const;
        size_t GetParameterCount() const;
        VariableType GetParameterType(_In_ size_t index) const;

        // Execute method
        HRESULT Invoke(_In_opt_ Object* instance, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue);
        HRESULT Invoke(_In_opt_ ObjectInstance* instance, _In_ size_t numParams, _In_ Var** parameters, _Out_opt_ Var* returnValue);

    private:
        VariableType _returnType;
        std::vector<VariableType> _parameterTypes;
        ScriptMethod _entryPoint;

        union
        {
            ScriptNoArguments       _noArgs;
            ScriptOneArgument       _oneArg;
            ScriptTwoArguments      _twoArgs;
            ScriptThreeArguments    _threeArgs;
            ScriptFourArguments     _fourArgs;
            ScriptMultiArguments    _multiArgs;
        };
    };
} // GDK
