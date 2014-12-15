#pragma once

#include "Value.h"

namespace GDK
{
    enum MethodFlag
    {
        MethodFlag_Constructor = 0x01,
        MethodFlag_Instance = 0x02,
        MethodFlag_Static = 0x04,
    };

    typedef HRESULT (*ScriptMethod)(_In_ size_t numParameters, _In_opt_count_(numParameters) Value* parameters, _Out_opt_ Value* returnValue);

    class MethodInfo
    {
    public:
        // Construction
        static HRESULT Create(_In_ MethodFlag methodFlags, _In_ ValueType returnType, _In_opt_ const char* name, _In_ uint16_t numParameters, _In_opt_count_(numParameters) ValueType* parameterTypes, _In_opt_count_(numParameters) const char** parameterNames, _In_ ScriptMethod entryPoint, MethodInfo** methodInfo);

        // Destruction
        ~MethodInfo();

        // Properties
        MethodFlag GetMethodFlags() const;
        ValueType GetReturnType() const;
        const char* GetName() const;
        uint16_t GetNumParameters() const;
        const ValueType* GetParameterTypes() const;
        const char* const* GetParameterNames() const;

        HRESULT InvokeMethod(_In_ size_t numParameters, _In_opt_count_(numParameters) Value* parameters, _Out_opt_ Value* returnValue);

    private:
        MethodInfo(_In_ MethodFlag methodFlags, _In_ ValueType returnType, _In_opt_ const char* name, _In_ uint16_t numParameters, _In_opt_count_(numParameters) ValueType* parameterTypes, _In_opt_count_(numParameters) const char** parameterNames, _In_ ScriptMethod entryPoint);

        MethodInfo(const MethodInfo&);
        MethodInfo& operator=(const MethodInfo&);

        MethodFlag _methodFlags;
        ValueType _returnType;
        char* _name;
        uint16_t _numParameters;
        ValueType* _parameterTypes;
        char** _parameterNames;

        ScriptMethod _entryPoint;
    };
} // GDK
