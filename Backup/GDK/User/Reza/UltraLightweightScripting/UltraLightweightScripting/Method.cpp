#include "StdAfx.h"
#include "Method.h"

using GDK::ScriptNoArguments;
using GDK::ScriptOneArgument;
using GDK::ScriptTwoArguments;
using GDK::ScriptThreeArguments;
using GDK::ScriptFourArguments;
using GDK::ScriptMultiArguments;
using GDK::VariableType;
using GDK::Variable;
using GDK::ScriptMethod;
using GDK::Method;
using GDK::ObjectInstance;
using GDK::Var;

Method::Method(_In_ VariableType returnType, _In_ size_t numParams, _In_ VariableType* parameterTypes, _In_ ScriptMethod entryPoint)
    : _returnType(returnType), _entryPoint(entryPoint)
{
    _parameterTypes.resize(numParams);
    for (size_t i = 0; i < numParams; ++i)
    {
        _parameterTypes[i] = parameterTypes[i];
    }
}

Method::Method(_In_ VariableType returnType, _In_ ScriptNoArguments entryPoint)
    : _returnType(returnType), _noArgs(entryPoint)
{
}

Method::Method(_In_ VariableType returnType, _In_ ScriptOneArgument entryPoint)
    : _returnType(returnType), _oneArg(entryPoint)
{
    _parameterTypes.resize(1);
}

Method::Method(_In_ VariableType returnType, _In_ ScriptTwoArguments entryPoint)
    : _returnType(returnType), _twoArgs(entryPoint)
{
    _parameterTypes.resize(2);
}

Method::Method(_In_ VariableType returnType, _In_ ScriptThreeArguments entryPoint)
    : _returnType(returnType), _threeArgs(entryPoint)
{
    _parameterTypes.resize(3);
}

Method::Method(_In_ VariableType returnType, _In_ ScriptFourArguments entryPoint)
    : _returnType(returnType), _fourArgs(entryPoint)
{
    _parameterTypes.resize(4);
}

Method::Method(_In_ VariableType returnType, _In_ ScriptMultiArguments entryPoint, _In_ size_t numArgs)
    : _returnType(returnType), _multiArgs(entryPoint)
{
    _parameterTypes.resize(numArgs);
}

Method::~Method()
{
}

VariableType Method::GetReturnType() const
{
    return _returnType;
}

size_t Method::GetParameterCount() const
{
    return _parameterTypes.size();
}

VariableType Method::GetParameterType(_In_ size_t index) const
{
    if (index >= _parameterTypes.size())
        return VariableType_Empty;

    return _parameterTypes[index];
}

HRESULT Method::Invoke(_In_opt_ GDK::Object* instance, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue)
{
    if (numParams != _parameterTypes.size())
        return E_INVALIDARG;

    for (size_t i = 0; i < numParams; ++i)
    {
        if (_parameterTypes[i] != parameters[i]->GetType())
            return E_INVALIDARG;
    }

    return _entryPoint(instance, numParams, parameters, returnValue);
}

HRESULT Method::Invoke(_In_opt_ ObjectInstance* instance, _In_ size_t numParams, _In_ Var** parameters, _Out_opt_ Var* returnValue)
{
    if (numParams != _parameterTypes.size())
        return E_INVALIDARG;

    switch (numParams)
    {
    case 0:     return _noArgs(instance, returnValue);
    case 1:     return _oneArg(instance, parameters[0], returnValue);
    case 2:     return _twoArgs(instance, parameters[0], parameters[1], returnValue);
    case 3:     return _threeArgs(instance, parameters[0], parameters[1], parameters[2], returnValue);
    case 4:     return _fourArgs(instance, parameters[0], parameters[1], parameters[2], parameters[3], returnValue);
    default:    return _multiArgs(instance, numParams, parameters, returnValue);
    }
}
