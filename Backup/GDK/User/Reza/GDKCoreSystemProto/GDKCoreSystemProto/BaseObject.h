#pragma once

namespace GDK {

class BaseObject
{
public:
    // Ensure new and delete for this object go through the executive's memory manager and the object manager
    _Ret_ void* operator new (_In_ size_t bytes, _In_opt_z_ const char* name, _In_opt_z_ const char* tag = nullptr);
    void operator delete (_In_ void* p, _In_opt_z_ const char* name, _In_opt_z_ const char* tag);
    void operator delete (_In_ _Post_ptr_invalid_ void *p);

    const char* GetName() const;

protected:
    BaseObject();
    virtual ~BaseObject();

private:
    char* _name;
};

} // GDK
