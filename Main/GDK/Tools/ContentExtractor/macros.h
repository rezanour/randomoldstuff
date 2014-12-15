#pragma once

#define CHECKHR(stmt, msg) hr = (stmt); if (FAILED(hr)) { wprintf(L"%ws, error is %08.8lx", msg, hr); goto Exit;}
#define CHECKTRUE(stmt, msg) CHECKHR((stmt) ? S_OK : E_FAIL, msg)
#define CHECKPOINTER(p, msg) CHECKHR((p != nullptr) ? S_OK : E_POINTER, msg);
