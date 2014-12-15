#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_HOSTING_H_
#define _GDK_HOSTING_H_

namespace GDK
{
    enum HostType
    {
        HostType_Windows,
        HostType_Console,
        HostType_Mobile,
    };

    // Every host must implement this interface before it can use and be
    // used with the game engine.
    struct IGameHost : IRefCounted
    {
        // Identification
        GDK_IMETHOD_(HostType) GetType() const = 0;

        // Application control
        GDK_IMETHOD_(void) Exit(_In_ uint32_t code) = 0;

        // Logging and Debug output

        // when enabled, log messages are echoed as debug strings to the console/debugger
        GDK_IMETHOD_(void) EnableDebugPrint(_In_ bool enabled) = 0;

        // When non-null, the specified file will be created and used to flush log data to.
        // Only one log file may be set at a time, so setting one will override any previous log.
        // Passing null will turn off file-based logging.
        GDK_IMETHOD_(void) SetLogFile(_In_opt_ const wchar_t* name) = 0;

        // allows printf style messages
        virtual void __cdecl LogMessageF(_In_ const wchar_t* format, ...) = 0;

        // send message to logging system
        GDK_IMETHOD_(void) LogMessage(_In_ const wchar_t* message) = 0;
    };
}

#endif // _GDK_HOSTING_H_
