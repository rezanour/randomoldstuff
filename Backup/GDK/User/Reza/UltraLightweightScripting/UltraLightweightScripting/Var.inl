#pragma once

namespace GDK
{
#define REF_TYPE_MAP(type, varType) \
    template <> \
    inline VarType Var::TypeOf(_In_ const type&) \
    { \
        return varType; \
    }

    REF_TYPE_MAP(bool, VarType_Boolean);
    REF_TYPE_MAP(int16_t, VarType_Integer);
    REF_TYPE_MAP(uint16_t, VarType_Integer);
    REF_TYPE_MAP(int32_t, VarType_Integer);
    REF_TYPE_MAP(uint32_t, VarType_Integer);
    REF_TYPE_MAP(int64_t, VarType_Integer);
    REF_TYPE_MAP(float, VarType_Real);
    REF_TYPE_MAP(double, VarType_Real);

    REF_TYPE_MAP(uint64_t, VarType_StringID);

#undef REF_TYPE_MAP

#define PTR_TYPE_MAP(type, varType) \
    template <> \
    inline VarType Var::TypeOf(_In_ const type*) \
    { \
        return varType; \
    }

    PTR_TYPE_MAP(ObjectInstance, VarType_Object);

#undef PTR_TYPE_MAP

    template <typename Ty>
    inline void Var::Set(_In_ const Ty& value)
    {
        if (_type == VarType_Empty)
            _type = TypeOf(value);

        if (IsCompatible(TypeOf(value)))
        {
            switch (_type)
            {
            case VarType_Boolean:
                _boolean = value != static_cast<Ty>(0);
                break;
            case VarType_Integer:
                _integer = static_cast<int64_t>(value);
                break;
            case VarType_Real:
                _real = static_cast<double>(value);
                break;
            case VarType_StringID:
                _stringID = static_cast<uint64_t>(value);
                break;
            default:
                assert(false && "invalid type");
            }
        }
    }

    template <typename Ty>
    inline void Var::Set(_In_ const Ty* value)
    {
        if (_type == VarType_Empty)
            _type = TypeOf(value);

        if (IsCompatible(TypeOf(value)))
        {
            switch (_type)
            {
            case VarType_Object:
                SafeRelease(_object);
                _object = static_cast<ObjectInstance*>(value);
                if (_object)
                {
                    _object->AddRef();
                }
                break;
            default:
                assert(false && "invalid type");
            }
        }
    }

} // GDK
