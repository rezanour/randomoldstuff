#pragma once

namespace GDK
{
    class Variable;
    class Method;

    class Object
    {
    public:
        static HRESULT Create(const std::wstring& name, Object** object);
        ~Object();

        uint32_t GetID() const;
        const wchar_t* GetName() const;

        size_t GetPropertyCount() const;
        const wchar_t* GetPropertyName(size_t index) const;
        std::shared_ptr<Variable> GetProperty(const std::wstring& name);
        std::shared_ptr<Variable> GetProperty(size_t index);

        size_t GetMethodCount() const;
        const wchar_t* GetMethodName(size_t index) const;
        std::shared_ptr<Method> GetMethod(const std::wstring& name);
        std::shared_ptr<Method> GetMethod(size_t index);

        HRESULT InvokeMethod(_In_ const std::wstring& name, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue);

    private:
        Object(const std::wstring& name);
        Object(const Object&);
        Object& operator= (const Object&);

        static uint32_t s_nextID;
        typedef std::map<std::wstring, std::shared_ptr<Variable>> PropertyMap;
        typedef std::map<std::wstring, std::shared_ptr<Method>> MethodMap;

        uint32_t _id;
        std::wstring _name;
        PropertyMap _properties;
        MethodMap _methods;
    };
} // GDK
