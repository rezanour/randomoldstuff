#pragma once

#if defined(_DEBUG)

void __cdecl DebugOut(_In_z_ _Printf_format_string_ const char* format, _In_ ...);

_Analysis_noreturn_
void __cdecl Error(_In_z_ _Printf_format_string_ const char* format, _In_ ...);

#define CHECKHR(x) { HRESULT hr_ = (x); if (FAILED(hr_)) { Error("%s failed with hr = 0x%08x.", #x, hr_); }}
#define CHECKGLE(x) { if (!(x)) { Error("%s failed with gle = %d.", #x, GetLastError()); }}
#define CHECKNOTNULL(x) { if (!(x)) { Error("%s is null.", #x); }}
#define CHECKTRUE(x) { if (!(x)) { Error("%s is false.", #x); }}

#else

#define DebugOut(format, ...)
#define Error(...) throw std::exception();
#define CHECKHR(x) { if (FAILED((x))) { Error(); }}
#define CHECKGLE(x) { if (!(x)) { Error(); } }
#define CHECKNOTNULL(x) { if (!(x)) { Error(); }}
#define CHECKTRUE(x) { if (!(x)) { Error(); }}

#endif
