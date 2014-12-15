#pragma once

#ifdef _DEBUG

void __cdecl DebugOut(_In_z_ _Printf_format_string_ const wchar_t* format, _Printf_format_string_params_(format) ...);

#else

#define DebugOut(format, ...) do { } while(0)

#endif
