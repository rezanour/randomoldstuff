#pragma once

#include "BaseObject.h"

namespace GDK {

class TaskManager : public BaseObject
{
public:
    static _Check_return_ HRESULT Create(_Deref_out_ TaskManager** ppTaskMgr);
    ~TaskManager();

private:
    TaskManager();
};

} // GDK
