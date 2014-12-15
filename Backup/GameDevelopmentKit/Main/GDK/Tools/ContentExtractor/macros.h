#pragma once

#define CHECKHR(stmt, msg) hr = (stmt); if (FAILED(hr)) { wprintf(L"%ws, error is %08.8lx", msg, hr); goto Exit;}