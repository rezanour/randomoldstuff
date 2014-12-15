#include "StdAfx.h"
#include "MethodInfo.h"

using GDK::MethodFlag;
using GDK::ScriptMethod;
using GDK::MethodInfo;
using GDK::ValueType;
using GDK::Value;

MethodInfo::MethodInfo(_In_ MethodFlag methodFlags, _In_ ValueType returnType, _In_opt_ const char* name, _In_ uint16_t numParameters, _In_opt_count_(numParameters) ValueType* parameterTypes, _In_opt_count_(numParameters) const char** parameterNames, _In_ ScriptMethod entryPoint)
    : _methodFlags(methodFlags), _returnType(returnType), _name(_strdup(name)), _numParameters(numParameters), _parameterTypes(nullptr), _parameterNames(nullptr), _entryPoint(entryPoint)
{
    if (numParameters > 0)
    {
        _parameterTypes = new ValueType[numParameters];
        for (size_t i = 0; i < _numParameters; ++i)
        {
            _parameterTypes[i] = parameterTypes[i];
        }

        _parameterNames = new char*[numParameters];
        for (size_t i = 0; i < _numParameters; ++i)
        {
            _parameterNames[i] = _strdup(parameterNames[i]);
        }
    }
}

MethodInfo::~MethodInfo()
{
    if (_parameterTypes)
    {
        delete [] _parameterTypes;
    }

    if (_parameterNames)
    {
        for (size_t i = 0; i < _numParameters; ++i)
        {
            free(_parameterNames[i]);
        }

        delete [] _parameterNames;
    }

    if (_name)
    {
        free(_name);
    }
}

HRESULT MethodInfo::Create(_In_ MethodFlag methodFlags, _In_ ValueType returnType, _In_opt_ const char* name, _In_ uint16_t numParameters, _In_opt_count_(numParameters) ValueType* parameterTypes, _In_opt_count_(numParameters) const char** parameterNames, _In_ ScriptMethod entryPoint, MethodInfo** methodInfo)
{
    if (!methodInfo)
        return E_POINTER;

    *methodInfo = new MethodInfo(methodFlags, returnType, name, numParameters, parameterTypes, parameterNames, entryPoint);
    return (*methodInfo) ? S_OK : E_OUTOFMEMORY;
}

MethodFlag MethodInfo::GetMethodFlags() const
{
    return _methodFlags;
}

ValueType MethodInfo::GetReturnType() const
{
    return _returnType;
}

const char* MethodInfo::GetName() const
{
    return _name;
}

uint16_t MethodInfo::GetNumParameters() const
{
    return _numParameters;
}

const ValueType* MethodInfo::GetParameterTypes() const
{
    return _parameterTypes;
}

const char* const* MethodInfo::GetParameterNames() const
{
    return _parameterNames;
}

HRESULT MethodInfo::InvokeMethod(_In_ size_t numParameters, _In_opt_count_(numParameters) Value* parameters, _Out_opt_ Value* returnValue)
{
    // instance methods must have the implicit 'this' at the beginning
    size_t expectedParameters = (_methodFlags & MethodFlag_Instance) ? _numParameters + 1 : _numParameters;

    if (expectedParameters != numParameters)
    {
        return E_INVALIDARG;
    }

    if (numParameters > 0 && !parameters)
    {
        return E_INVALIDARG;
    }

    return _entryPoint(numParameters, parameters, returnValue);
}
