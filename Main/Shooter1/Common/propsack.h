#pragma once

#pragma pack(push,1)

enum class ValueType
{
    Unknown,
    Int,
    String,
    Float,
    WideString,
    Bool
};

struct PropValue
{
    PropValue()
    {
        vType = ValueType::Unknown;
        iVal = 0;
    }
    ValueType vType;
    union
    {
        int64_t iVal;
        double fVal;
        bool bVal;
        wchar_t* wsVal;
        char* sVal;
    };
};

#pragma pack(pop)

typedef std::map<std::string, PropValue> PropValues;
typedef std::map<std::string, PropValues> SackOfProps; 

class PropSack
{
public:
    ~PropSack();

    static std::shared_ptr<PropSack> LoadFromFile(const wchar_t* path);
    static void Clear(PropValue& val);
    void SaveToFile(const wchar_t* path);

public:
    void AddValue(const char* object, const char* prop, PropValue& val, bool takeOwnership = false);
    void AddIntValue(const char* object, const char* prop, int64_t val);
    void AddFloatValue(const char* object, const char* prop, double val);
    void AddStringValue(const char* object, const char* prop, char* val);
    void AddStringValue(const char* object, const char* prop, wchar_t* val);
    void AddBoolValue(const char* object, const char* prop, bool val);
    void RemoveObject(const char* object);

    PropValue& GetValue(const char* object, const char* prop);

    // Extended Methods
    char* GetStringValue(const char* object, const char* prop);
    wchar_t* GetStringValueW(const char* object, const char* prop);
    int64_t GetIntValue(const char* object, const char* prop, int64_t defVal = 0);
    double GetFloatValue(const char* object, const char* prop, double defVal = 0.0);
    bool GetBoolValue(const char* object, const char* prop, bool defVal = false);

private:
    SackOfProps sack;
};
