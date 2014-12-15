// Memory reading and writing using IStream
#pragma once

#include <Windows.h>
#include <vector>
#include <stde\win32handle.h>
#include <stde\com_ptr.h>

namespace CoreServices
{
    class SubStream : public IStream
    {
    public:
        SubStream(_In_ IStream* pStream, _In_ size_t length);
        virtual ~SubStream();

        // Static methods
        static HRESULT Create(_In_ IStream* pStream, _In_ size_t length, _Deref_out_ IStream** ppStream);
        static HRESULT Seek(_In_ IStream* pStream, DWORD dwOrigin, LONG offset);

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
        ULONGLONG GetStreamTotalSizeBytes();
    private:
        long _refcount;
        ULARGE_INTEGER _CurrentPosition;
        ULARGE_INTEGER _backingStreamCurrentPosition;
        stde::com_ptr<IStream> _spStream;
        size_t _cbMemory;
    };
};