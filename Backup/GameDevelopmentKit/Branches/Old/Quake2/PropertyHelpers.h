#ifndef _PROPERTYHELPERS_H_
#define _PROPERTYHELPERS_H_

//
// A collection of simple property implementations
//

namespace Quake2
{
    //
    // String Property
    //

    class StringProperty : public GDK::RuntimeObject<GDK::IStringProperty>
    {
    public:
        StringProperty(_In_z_ const wchar_t* name, _In_ std::wstring* target) :
            _name(name),
            _target(target)
        {
        }

        virtual const wchar_t* GDKAPI GetName() const
        {
            return _name.c_str();
        }

        virtual HRESULT GDKAPI ToString(_COM_Outptr_ GDK::IString** string)
        {
            if (!string)
            {
                return E_INVALIDARG;
            }

            return GDK::CreateString(_target->c_str(), string);
        }

        virtual const wchar_t* GDKAPI GetStringValue()
        {
            return _target->c_str();
        }

        virtual void GDKAPI SetStringValue(_In_z_ const wchar_t* value)
        {
            *_target = value;
        }

    private:
        std::wstring _name;
        std::wstring* _target;
    };

    //
    // Integer Property
    //

    class IntegerProperty : public GDK::RuntimeObject<GDK::IIntegerProperty>
    {
    public:
        IntegerProperty(_In_z_ const wchar_t* name, _In_ int64_t* target) :
            _name(name),
            _target(target)
        {
        }

        virtual const wchar_t* GDKAPI GetName() const
        {
            return _name.c_str();
        }

        virtual HRESULT GDKAPI ToString(_COM_Outptr_ GDK::IString** string)
        {
            if (!string)
            {
                return E_INVALIDARG;
            }

            return GDK::CreateString(GDK::ConvertToString(*_target).c_str(), string);
        }

        virtual int64_t GDKAPI GetIntegerValue()
        {
            return *_target;
        }

        virtual void GDKAPI SetIntegerValue(_In_ int64_t value)
        {
            *_target = value;
        }

    private:
        std::wstring _name;
        int64_t* _target;
    };

} // Quake2

#endif // _PROPERTYHELPERS_H_