#include "StdAfx.h"
#include "FileStream.h"
#include "Debug.h"

using namespace CoreServices;

FileStream::FileStream(_In_ HANDLE file) :
    _refcount(1),
    _file(file),
    _attached(false)
{

}

FileStream::FileStream(_In_ HANDLE file, _In_ bool attached) :
    _refcount(1),
    _attached(attached)
{
    _file.attach(file);
}

FileStream::~FileStream()
{
    // avoid automatically closing attached files
    if (_attached)
    {
        // HANDLE is abandoned because it is assumed to be owned by the caller, not this stream instance
        _file.detach(); 
    }
}

// Static methods
HRESULT FileStream::Create(_In_ const std::wstring& filePath, _In_ bool readOnly, _Deref_out_ IStream** ppStream)
{
    if (ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppStream = nullptr;

    HRESULT hr = S_OK;
    stde::file_handle file(::CreateFileW(filePath.c_str(), readOnly ? GENERIC_READ : GENERIC_WRITE, readOnly ? FILE_SHARE_READ : FILE_SHARE_WRITE, nullptr, readOnly ? OPEN_EXISTING : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));

    if (file.valid())
    {
        *ppStream = new FileStream(file);
        if(*ppStream == nullptr)
        {
            hr = E_OUTOFMEMORY;
        }

        file.detach(); // HANDLE is now owned by the FileStream instance
    }
    else
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

HRESULT FileStream::Attach(_In_ HANDLE file, _Deref_out_ IStream** ppStream)
{
    if (ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppStream = nullptr;

    HRESULT hr = S_OK;

    *ppStream = new FileStream(file, true);
    if(*ppStream == nullptr)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT FileStream::Seek(_In_ IStream* pStream, DWORD dwOrigin, LONG offset)
{
    if (pStream == nullptr)
    {
        return E_INVALIDARG;
    }

    LARGE_INTEGER liMove = {0};
    ULARGE_INTEGER uliPos = {0};
    liMove.LowPart = offset;

    return pStream->Seek(liMove, dwOrigin, &uliPos);
}

uint64 FileStream::GetLength(_In_ IStream* pStream)
{
    HRESULT hr = S_OK;
    uint64 length = 0;
    STATSTG stats = {0};

    ISNOTNULL(pStream, E_INVALIDARG);
    CHECKHR(pStream->Stat(&stats, STATFLAG_DEFAULT));
    length = stats.cbSize.QuadPart;

EXIT
    return length;
}

bool FileStream::FileExists(_In_ const std::wstring& filePath)
{
    return (GetFileAttributes(filePath.c_str()) != INVALID_FILE_ATTRIBUTES);
}

// IUnknown
HRESULT STDMETHODCALLTYPE FileStream::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    if (iid == __uuidof(IStream))
    {
        *ppvObject = static_cast<IStream*>(this);
        AddRef();
        return S_OK;
    }

    if (iid == __uuidof(ISequentialStream))
    {
        *ppvObject = static_cast<ISequentialStream*>(this);
        AddRef();
        return S_OK;
    }
        
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE FileStream::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE FileStream::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

// ISequentialStream
HRESULT STDMETHODCALLTYPE FileStream::Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead)
{
    BOOL rc = ReadFile(_file, pv, cb, pcbRead, nullptr);
    return (rc) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}

HRESULT STDMETHODCALLTYPE FileStream::Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten)
{
    BOOL rc = WriteFile(_file, pv, cb, pcbWritten, nullptr);
    return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}

// IStream
HRESULT STDMETHODCALLTYPE FileStream::SetSize(_In_ ULARGE_INTEGER libNewSize)
{
    UNREFERENCED_PARAMETER(libNewSize);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::CopyTo(_In_ IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten)
{
    UNREFERENCED_PARAMETER(pStream);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(pcbRead);
    UNREFERENCED_PARAMETER(pcbWritten);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::Commit(_In_ DWORD grfCommitFlags)
{
    UNREFERENCED_PARAMETER(grfCommitFlags);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::Revert()
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::Clone(_Deref_out_ IStream **ppStream)
{
    UNREFERENCED_PARAMETER(ppStream);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE FileStream::Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* lpNewFilePointer)
{
    DWORD dwMoveMethod = FILE_BEGIN;

    switch(dwOrigin)
    {
    case STREAM_SEEK_SET:
        dwMoveMethod = FILE_BEGIN;
        break;
    case STREAM_SEEK_CUR:
        dwMoveMethod = FILE_CURRENT;
        break;
    case STREAM_SEEK_END:
        dwMoveMethod = FILE_END;
        break;
    default:
        return STG_E_INVALIDFUNCTION;
        break;
    }

    if (SetFilePointerEx(_file, liDistanceToMove, (PLARGE_INTEGER) lpNewFilePointer, dwMoveMethod) == 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE FileStream::Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag)
{
    if (pStatstg == nullptr)
    {
        return E_INVALIDARG;
    }

    UNREFERENCED_PARAMETER(grfStatFlag);

    if (GetFileSizeEx(_file, (PLARGE_INTEGER) &pStatstg->cbSize) == 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    return S_OK;
}
