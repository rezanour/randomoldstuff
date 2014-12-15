#include "Precomp.h"
#include <GDKString.h>
#include <string>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class GDKString : public RuntimeObject<IString>
    {
    public:
        static HRESULT GDKAPI Create(_In_opt_z_ const wchar_t* value, _COM_Outptr_ IString** string)
        {
            MODULE_GUARD_BEGIN

            CHECK_NOT_NULL(string, E_POINTER);
            *string = nullptr;

            ComPtr<GDKString> str = Make<GDKString>(value);
            CHECKHR(str.CopyTo(string));

            MODULE_GUARD_END
        }

        GDKString(_In_opt_z_ const wchar_t* value) :
            _value(value)
        {
        }

        virtual bool GDKAPI IsNullOrEmpty() const
        {
            return _value.empty();
        }

        virtual size_t GDKAPI GetLength() const
        {
            return _value.size();
        }

        virtual const wchar_t* GDKAPI GetBuffer() const
        {
            return _value.c_str();
        }

    private:
        std::wstring _value;
    };

    class GDKStringList : public RuntimeObject<IStringList>
    {
    public:
        static HRESULT GDKAPI Create(_In_opt_z_count_(count) const wchar_t** values, _In_ size_t count, _COM_Outptr_ IStringList** stringList)
        {
            MODULE_GUARD_BEGIN

            CHECK_NOT_NULL(stringList, E_POINTER);
            *stringList = nullptr;

            CHECK_IS_TRUE(values != nullptr || count == 0, E_INVALIDARG);

            ComPtr<GDKStringList> list = Make<GDKStringList>(values, count);
            CHECKHR(list.CopyTo(stringList));

            MODULE_GUARD_END
        }

        GDKStringList(_In_opt_z_count_(count) const wchar_t** values, _In_ size_t count)
        {
            for (size_t i = 0; i < count; ++i)
            {
                ComPtr<IString> string;
                CHECKHR(CreateString(values[i], string.GetAddressOf()));
                _values.push_back(string);
            }
        }

        virtual size_t GDKAPI GetCount() const
        {
            return _values.size();
        }

        virtual HRESULT GDKAPI GetAt(_In_ size_t index, _COM_Outptr_ IString** string) const
        {
            MODULE_GUARD_BEGIN

            CHECK_NOT_NULL(string, E_POINTER);
            *string = nullptr;

            CHECK_IS_TRUE(index < _values.size(), E_INVALIDARG);

            CHECKHR(_values[index].CopyTo(string));

            MODULE_GUARD_END
        }

    private:
        std::vector<ComPtr<IString>> _values;
    };

    extern "C" HRESULT CreateString(_In_opt_z_ const wchar_t* value, _COM_Outptr_ IString** string)
    {
        return GDKString::Create(value, string);
    }

    extern "C" HRESULT CreateStringList(_In_opt_z_count_(count) const wchar_t** values, _In_ size_t count, _COM_Outptr_ IStringList** stringList)
    {
        return GDKStringList::Create(values, count, stringList);
    }

}