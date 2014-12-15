#pragma once

const int VT_FLOAT3 = 1000;
const int VT_FLOAT4 = 1001;

enum PropertyType
{
    StringProperty = VT_BSTR,
    I1Property     = VT_I1,
    I2Property     = VT_I2,
    I4Property     = VT_I4,
    I8Property     = VT_I8,
    R4Property     = VT_R4,
    R8Property     = VT_R8,
    UI1Property    = VT_UI1,
    UI2Property    = VT_UI2,
    UI4Property    = VT_UI4,
    UI8Property    = VT_UI8,
    Float3Property = VT_FLOAT3,
    Float4Property = VT_FLOAT4,
};

struct PropertyInfo
{
    PropertyType type;
};

class PropertyEngine
{
public:
    PropertyEngine();
    virtual ~PropertyEngine();

    PropertyType TypeFromId(LPCWSTR name) { return _typeMap[name]; }

private:
    std::map< std::wstring, PropertyType > _typeMap; 
};