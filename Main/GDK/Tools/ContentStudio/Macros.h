#pragma once
#define IfFailedGotoExit(EXPR) do { hr = (EXPR); if (FAILED(hr)) { goto Exit; }} while(FALSE, FALSE)
#define IfNullGotoExit(EXPR, ERROR) do { if (!(EXPR)) { hr = (ERROR); goto Exit; } } while(FALSE, FALSE)
