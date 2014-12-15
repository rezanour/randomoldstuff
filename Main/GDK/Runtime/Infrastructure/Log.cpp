#include <Log.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

void GDK::Log::WriteInfo(_In_ const std::wstring& message)
{
#ifdef WIN32
    OutputDebugStringW(message.c_str());
#endif

    std::cout << message.c_str() << std::endl;
}