#include "StdAfx.h"
#include "BaseObject.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "ObjectManager.h"

namespace GDK {

BaseObject::BaseObject()
    : _name(nullptr)
{
}

BaseObject::~BaseObject()
{
    if (_name)
    {
        Executive::GetExecutive()->GetMemoryManager()->Free(_name);
        _name = nullptr;
    }
}

void* BaseObject::operator new(size_t bytes, const char* name, const char* tag)
{
    Executive* exec = Executive::GetExecutive();
    if (!exec)
    {
        return nullptr;
    }

    BaseObject* pObject = nullptr;
    HRESULT hr = exec->GetObjectManager()->CreateObject(name ? name : tag, bytes, &pObject);
    if (FAILED(hr))
    {
        // TODO: Error!
        OutputDebugString(L"Failed to allocated object!");
        pObject = nullptr;
    }
    else if (name)
    {
        char customTag[50] = {0};
        sprintf_s(customTag, _countof(customTag), "BaseObject::_name - ", name);

        if (strlen(name) < 25)
        {
            strcat_s(customTag, _countof(customTag), name);
        }

        pObject->_name = static_cast<char*>(exec->GetMemoryManager()->Alloc(strlen(name) + 1, customTag));
        strcpy_s(pObject->_name, strlen(name) + 1, name);
    }

    return reinterpret_cast<void*>(pObject);
}

void BaseObject::operator delete(void* p, const char* name, const char* tag)
{
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(tag);
    delete p;
}

void BaseObject::operator delete(void* p)
{
    Executive* exec = Executive::GetExecutive();
    if (!exec)
    {
        // TODO: Error!
        OutputDebugString(L"Failed to free memory, Executive is not running!\n");
    }

    exec->GetObjectManager()->FreeObject(static_cast<BaseObject*>(p));
}

const char* BaseObject::GetName() const
{
    return _name;
}

} // GDK
