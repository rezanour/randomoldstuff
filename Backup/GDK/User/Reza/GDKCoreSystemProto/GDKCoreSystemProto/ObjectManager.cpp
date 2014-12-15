#include "StdAfx.h"
#include "Executive.h"
#include "ObjectManager.h"
#include "MemoryManager.h"

namespace GDK {

ObjectManager::ObjectManager()
    : _maxObjects(100), _numObjects(0)
{
    _objects = static_cast<BaseObject**>(Executive::GetExecutive()->GetMemoryManager()->Alloc(sizeof(BaseObject*) * _maxObjects, "ObjectManager::_objects"));
    for (size_t i = 0; i < _maxObjects; ++i)
    {
        _objects[i] = nullptr;
    }
}

ObjectManager::~ObjectManager()
{
    if (_objects)
    {
#ifndef NDEBUG
        for (size_t i = 0; i < _numObjects; ++i)
        {
            wprintf(L"Object: %S\n", _objects[i]->GetName());
        }
#endif

        Executive::GetExecutive()->GetMemoryManager()->Free(_objects);
        _objects = nullptr;
    }
}

void* ObjectManager::operator new(size_t bytes, const char* tag)
{
    Executive* exec = Executive::GetExecutive();
    if (!exec)
    {
        return nullptr;
    }

    return exec->GetMemoryManager()->Alloc(bytes, tag);
}

void ObjectManager::operator delete(void* p, const char* tag)
{
    UNREFERENCED_PARAMETER(tag);
    delete p;
}

void ObjectManager::operator delete(void* p)
{
    Executive* exec = Executive::GetExecutive();
    if (!exec)
    {
        // TODO: Error!
        OutputDebugString(L"Failed to free memory, Executive is not running!\n");
    }

    exec->GetMemoryManager()->Free(p);
}

HRESULT ObjectManager::Create(ObjectManager** ppObjMgr)
{
    if (!ppObjMgr)
        return E_POINTER;

    *ppObjMgr = new("ObjectManager") ObjectManager;
    HRESULT hr = (*ppObjMgr) ? S_OK : E_OUTOFMEMORY;

    return hr;
}

HRESULT ObjectManager::CreateObject(const char* name, size_t bytes, BaseObject** ppObject)
{
    if (!ppObject)
        return E_POINTER;

    const char* tag = name ? name : "<unnamed object>";

    *ppObject = static_cast<BaseObject*>(Executive::GetExecutive()->GetMemoryManager()->Alloc(bytes, tag));
    HRESULT hr = (*ppObject) ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        _objects[_numObjects++] = *ppObject;
    }

    return hr;
}

void ObjectManager::FreeObject(BaseObject* pObject)
{
    for (size_t i = 0; i < _numObjects; ++i)
    {
        if (_objects[i] == pObject)
        {
            --_numObjects;
            for (size_t j = i; j < _numObjects; ++j)
            {
                _objects[j] = _objects[j + 1];
            }
            break;
        }
    }

    Executive::GetExecutive()->GetMemoryManager()->Free(pObject);
}

} // GDK
