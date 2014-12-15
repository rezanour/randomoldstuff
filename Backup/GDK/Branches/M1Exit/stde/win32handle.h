// Win32Handle.h is a set of helper functionality to make working with Win32 HANDLE's more convenient

#pragma once

#ifdef WIN32

#include <Windows.h>

namespace stde
{
    // Use traits pattern for supporting various handle semantics

    struct file_handle_traits
    {
        static HANDLE __fastcall    invalid()                   { return INVALID_HANDLE_VALUE; }
        static void __fastcall      close(_In_ HANDLE handle)   { CloseHandle(handle); }
    };

    struct find_file_handle_traits
    {
        static HANDLE __fastcall    invalid()                   { return INVALID_HANDLE_VALUE; }
        static void __fastcall      close(_In_ HANDLE handle)   { FindClose(handle); }
    };

    struct event_handle_traits
    {
        static HANDLE __fastcall    invalid()                   { return nullptr; }
        static void __fastcall      close(_In_ HANDLE handle)   { CloseHandle(handle); }
    };

    struct thread_handle_traits
    {
        static HANDLE __fastcall    invalid()                   { return nullptr; }
        static void __fastcall      close(_In_ HANDLE handle)   { CloseHandle(handle); } 
    };

    // forward declare for typedefs
    template <typename Traits> class unique_handle;

    typedef unique_handle<file_handle_traits>       file_handle;
    typedef unique_handle<event_handle_traits>      event_handle;
    typedef unique_handle<thread_handle_traits>     thread_handle;
    typedef unique_handle<find_file_handle_traits>  find_file_handle;

    // unique handle object which closes the handle when it goes out of scope.
    // Note, because this is designed to be 'unique', it cannot be used in collections, etc...
    template <typename Traits>
    class unique_handle sealed
    {
    public:
        explicit unique_handle(_In_ HANDLE handle = Traits::invalid())
            : handle(handle)
        {
            if (handle != Traits::invalid())
            {
                // prevent others from closing this specific handle.
                SetHandleInformation(handle, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE);
            }
        }

        ~unique_handle()
        {
            close();
        }

        void __fastcall attach(_In_ const HANDLE& handle) { this->handle = handle; }
        HANDLE __fastcall detach() { HANDLE old = handle; handle = Traits::invalid(); return old; }

        // typecast operator. 
        __fastcall operator const HANDLE& () const { return handle; }

        __fastcall operator bool() const { return handle != Traits::invalid(); }
        bool __fastcall valid() const { return handle != Traits::invalid(); }

        static HANDLE __fastcall invalid_value() { return Traits::invalid(); }

    private:
        // prevent the unique_handle object from being copied.
        unique_handle(unique_handle&);
        unique_handle& operator= (unique_handle&);

        void __fastcall close()
        {
            if (handle != Traits::invalid())
            {
                // Clear the lock so that we can close the handle
                SetHandleInformation(handle, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);

                Traits::close(handle);
                handle = Traits::invalid();
            }
        }

        HANDLE handle;
    };
}

#endif

