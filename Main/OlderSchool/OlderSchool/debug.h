#pragma once

void __cdecl DebugOut(_In_z_ _Printf_format_string_ const char* format, ...);
void __cdecl Error(_In_z_ _Printf_format_string_ const char* format, ...);

#if _DEBUG

#define CHECKHR(x) { HRESULT hr_ = (x);  if (FAILED(hr_)) Error("%s failed with hr = 0x%08x.", #x, hr_); }
#define CHECKGLE(x) if (!(x)) Error("%s failed with gle = %d.", #x, GetLastError());
#define CHECKNOTNULL(x) if (!(x)) Error("%s is nullptr.", #x);

#else

#define CHECKHR(x) { HRESULT hr_ = (x);  if (FAILED(hr_)) Error("0x%08x.", hr_); }
#define CHECKGLE(x) if (!(x)) Error("%d.", GetLastError());
#define CHECKNOTNULL(x) if (!(x)) Error("%s is nullptr.", #x);

#endif
