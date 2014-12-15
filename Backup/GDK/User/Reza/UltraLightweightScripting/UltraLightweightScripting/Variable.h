#pragma once

namespace GDK
{
    class Object;

    enum VariableType
    {
        VariableType_Empty = 0,
        VariableType_Boolean,
        VariableType_Integer,
        VariableType_Real,
        VariableType_Char,
        VariableType_String,
        VariableType_Object,
    };

    class Variable
    {
    public:
        Variable();
        Variable(VariableType type);
        explicit Variable(int8_t value);
        explicit Variable(int16_t value);
        explicit Variable(int32_t value);
        explicit Variable(int64_t value);
        explicit Variable(double value);
        explicit Variable(bool value);
        explicit Variable(wchar_t value);
        explicit Variable(const wchar_t* value);
        explicit Variable(Object* value);

        Variable(const Variable& other);
        Variable& operator= (const Variable& other);

        ~Variable();

        void Clear();

        VariableType GetType() const;

        bool IsEmpty() const;
        bool IsBoolean() const;
        bool IsInteger() const;
        bool IsReal() const;
        bool IsNumber() const;
        bool IsChar() const;
        bool IsString() const;
        bool IsObject() const;

        bool AsBoolean() const;
        int64_t AsInteger() const;
        double AsReal() const;
        wchar_t AsChar() const;
        const wchar_t* AsString() const;
        Object* AsObject() const;

        void ChangeType(VariableType type);

        void SetValue(bool value);
        void SetValue(int8_t value);
        void SetValue(int16_t value);
        void SetValue(int32_t value);
        void SetValue(int64_t value);
        void SetValue(double value);
        void SetValue(wchar_t value);
        void SetValue(const wchar_t* value);
        void SetValue(Object* value);

    private:
        void ClearValue();

        VariableType _type;
        union
        {
            bool _bool;
            int64_t _integer;
            double _real;
            wchar_t _char;
            wchar_t* _string;
            Object* _object;
        };
    };

} // GDK
