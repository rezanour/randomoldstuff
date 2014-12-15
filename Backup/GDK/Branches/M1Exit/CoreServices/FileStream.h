// File reading and writing using IStream
#pragma once

#include <Windows.h>
#include <string>
#include <stde\types.h>
#include <stde\win32handle.h>

namespace CoreServices
{
    class FileStream : public IStream
    {
    public:
        FileStream(_In_ HANDLE file);
        FileStream(_In_ HANDLE file, _In_ bool attached);
        virtual ~FileStream();

        // Static methods
        static HRESULT Create(_In_ const std::wstring& filePath, _In_ bool readOnly, _Deref_out_ IStream** ppStream);
        static HRESULT Attach(_In_ HANDLE file, _Deref_out_ IStream** ppStream);
        static HRESULT Seek(_In_ IStream* pStream, DWORD dwOrigin, LONG offset);
        static uint64 GetLength(_In_ IStream* pStream);

        // IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
        ULONG STDMETHODCALLTYPE AddRef();
        ULONG STDMETHODCALLTYPE Release();

        // ISequentialStream
        HRESULT STDMETHODCALLTYPE Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead);
        HRESULT STDMETHODCALLTYPE Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten);

        // IStream
        HRESULT STDMETHODCALLTYPE SetSize(_In_ ULARGE_INTEGER libNewSize);
        HRESULT STDMETHODCALLTYPE CopyTo(_In_ IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten);
        HRESULT STDMETHODCALLTYPE Commit(_In_ DWORD grfCommitFlags);
        HRESULT STDMETHODCALLTYPE Revert();
        HRESULT STDMETHODCALLTYPE LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType);
        HRESULT STDMETHODCALLTYPE UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType);
        HRESULT STDMETHODCALLTYPE Clone(_Deref_out_ IStream **ppStream);
        HRESULT STDMETHODCALLTYPE Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* lpNewFilePointer);
        HRESULT STDMETHODCALLTYPE Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag); 

    private:
        stde::file_handle _file;
        bool _attached;
        long _refcount;
    };
};