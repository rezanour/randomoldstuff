#pragma once

#include "BaseObject.h"

namespace GDK {

class ObjectManager
{
public:
    // Ensure new and delete for this object go through the executive's memory manager
    _Ret_ void* operator new (_In_ size_t bytes, _In_opt_z_ const char* tag = nullptr);
    void operator delete (_In_ void* p, _In_opt_z_ const char* tag);
    void operator delete (_In_ _Post_ptr_invalid_ void *p);

    // Construction/Destruction
    static _Check_return_ HRESULT Create(_Deref_out_ ObjectManager** ppObjMgr);
    ~ObjectManager();

    // Object operations
    _Check_return_ HRESULT CreateObject(_In_opt_z_ const char* name, _In_ size_t bytes, _Deref_out_ BaseObject** ppObject);
    void FreeObject(_In_ _Post_ptr_invalid_ BaseObject* pObject);

private:
    ObjectManager();

    BaseObject** _objects;
    size_t _maxObjects;
    size_t _numObjects;
};

} // GDK
