#include "StdAfx.h"
#include "Screen.h"
#include "ObjectNamespace.h"
#include "MemoryManager.h"
#include "MessageQueue.h"

using GDK::Screen;
using GDK::ObjectNamespace;
using GDK::MemoryManager;
using GDK::MessageQueue;

Screen::Screen(ObjectNamespace* globals)
    : _globals(globals), _objects(nullptr), _memPool(nullptr), _messageQueue(nullptr)
{
    assert(globals);
    globals->AddRef();
}

Screen::~Screen()
{
    SafeRelease(_globals);
    SafeRelease(_objects);
    SafeRelease(_messageQueue);
    SafeDelete(_memPool);
}

HRESULT Screen::Create(ObjectNamespace* globals, Screen** ppScreen)
{
    if (!globals)
        return E_INVALIDARG;

    if (!ppScreen)
        return E_POINTER;

    *ppScreen = nullptr;

    HRESULT hr = S_OK;

    Screen* screen = GDKNEW("Screen") Screen(globals);
    hr = screen ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        hr = ObjectNamespace::Create(&screen->_objects);
    }

    if (SUCCEEDED(hr))
    {
        hr = MemoryManager::Create(&screen->_memPool);
    }

    if (SUCCEEDED(hr))
    {
        hr = MessageQueue::Create(&screen->_messageQueue);
    }

    if (SUCCEEDED(hr))
    {
        *ppScreen = screen;
    }
    else if (screen)
    {
        screen->Release();
    }

    return hr;
}
