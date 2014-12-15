#include "StdAfx.h"
#include "TaskManager.h"

namespace GDK {

TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{
}

HRESULT TaskManager::Create(TaskManager** ppTaskMgr)
{
    if (!ppTaskMgr)
        return E_POINTER;

    *ppTaskMgr = new("TaskManager") TaskManager;
    HRESULT hr = (*ppTaskMgr) ? S_OK : E_OUTOFMEMORY;

    return hr;
}

} // GDK
