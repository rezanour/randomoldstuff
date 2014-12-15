#include "precomp.h"

std::shared_ptr<PropSack> PropSack::LoadFromFile(const wchar_t* path)
{
    UNREFERENCED_PARAMETER(path);
    return nullptr;
}

void PropSack::Clear(PropValue& val)
{
    if (val.vType == ValueType::String && val.sVal)
    {
        delete [] val.sVal;
        val.sVal = nullptr;
    }
    else if (val.vType == ValueType::WideString && val.wsVal)
    {
        delete [] val.wsVal;
        val.wsVal = nullptr;
    }
}

PropSack::~PropSack()
{
    for each (auto obj in sack)
    {
        RemoveObject(obj.first.c_str());
    }
}

void PropSack::AddValue(const char* object, const char* prop, PropValue& val, bool takeOwnership)
{
    if (takeOwnership)
    {
        sack[object][prop] = val;
        return;
    }

    switch(val.vType)
    {
    case ValueType::Int:
        return AddIntValue(object, prop, val.iVal);
    case ValueType::Float:
        return AddFloatValue(object, prop, val.fVal);
    case ValueType::Bool:
        return AddBoolValue(object, prop, val.bVal);
    case ValueType::String:
        return AddStringValue(object, prop, val.sVal);
    case ValueType::WideString:
        return AddStringValue(object, prop, val.wsVal);
    default:
    break;
    }
}

void PropSack::AddIntValue(const char* object, const char* prop, int64_t val)
{
    PropValue v;
    v.vType = ValueType::Int;
    v.iVal = val;
    sack[object][prop] = v;
}

void PropSack::AddFloatValue(const char* object, const char* prop, double val)
{
    PropValue v;
    v.vType = ValueType::Float;
    v.fVal = val;
    sack[object][prop] = v;
}
void PropSack::AddStringValue(const char* object, const char* prop, char* val)
{
    PropValue v;
    v.vType = ValueType::String;
    v.sVal = new char[strlen(val) + 1];
    lstrcpyA(v.sVal, val);
    sack[object][prop] = v;
}
void PropSack::AddStringValue(const char* object, const char* prop, wchar_t* val)
{
    PropValue v;
    v.vType = ValueType::WideString;
    v.wsVal = new wchar_t[wcslen(val) + 1];
    lstrcpyW(v.wsVal, val);
    sack[object][prop] = v;
}
void PropSack::AddBoolValue(const char* object, const char* prop, bool val)
{
    PropValue v;
    v.vType = ValueType::Bool;
    v.bVal = val;
    sack[object][prop] = v;
}

PropValue& PropSack::GetValue(const char* object, const char* prop)
{
    return sack[object][prop];
}

char* PropSack::GetStringValue(const char* object, const char* prop)
{
    PropValue v = GetValue(object,prop);
    if (v.vType == ValueType::String)
    {
        return v.sVal;
    }

    return nullptr;
}

wchar_t* PropSack::GetStringValueW(const char* object, const char* prop)
{
    PropValue v = GetValue(object,prop);
    if (v.vType == ValueType::WideString)
    {
        return v.wsVal;
    }

    return nullptr;
}

int64_t PropSack::GetIntValue(const char* object, const char* prop, int64_t defVal)
{
    PropValue v = GetValue(object,prop);
    if (v.vType == ValueType::Int)
    {
        return v.iVal;
    }

    return defVal;
}

double PropSack::GetFloatValue(const char* object, const char* prop, double defVal)
{
    PropValue v = GetValue(object,prop);
    if (v.vType == ValueType::Float)
    {
        return v.fVal;
    }

    return defVal;
}

bool PropSack::GetBoolValue(const char* object, const char* prop, bool defVal)
{
    PropValue v = GetValue(object,prop);
    if (v.vType == ValueType::Bool)
    {
        return v.bVal;
    }

    return defVal;
}

void PropSack::RemoveObject(const char* object)
{
    PropValues& values = sack[object];
    for each (auto val in values)
    {
        PropSack::Clear(val.second);
    }
    sack.erase(object);
}
