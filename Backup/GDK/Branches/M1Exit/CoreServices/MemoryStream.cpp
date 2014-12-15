#include "MemoryStream.h"
#include "Debug.h"

using namespace CoreServices;

MemoryStream::MemoryStream(_In_ size_t length) :
    _refcount(1),
    _pMemory(nullptr),
    _cbMemory(0)
{
    _CurrentPosition.QuadPart = 0;
    _memory.resize(length);
}

MemoryStream::MemoryStream(_In_ void* pBuffer, _In_ size_t length) :
    _refcount(1),
    _pMemory(pBuffer),
    _cbMemory(length)
{
    _CurrentPosition.QuadPart = 0;
}

MemoryStream::~MemoryStream()
{
    
}

// Static methods
HRESULT MemoryStream::Create(_In_ size_t length, _Deref_out_ IStream** ppStream)
{
    if (ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppStream = nullptr;

    HRESULT hr = S_OK;

    *ppStream = new MemoryStream(length);
    if(*ppStream == nullptr)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT MemoryStream::Attach(_In_ void* pBuffer, _In_ size_t length, _Deref_out_ IStream** ppStream)
{
    if (ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppStream = nullptr;

    HRESULT hr = S_OK;

    *ppStream = new MemoryStream(pBuffer, length);
    if(*ppStream == nullptr)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT MemoryStream::Seek(_In_ IStream* pStream, DWORD dwOrigin, LONG offset)
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

// IUnknown
HRESULT STDMETHODCALLTYPE MemoryStream::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE MemoryStream::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE MemoryStream::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

// ISequentialStream
HRESULT STDMETHODCALLTYPE MemoryStream::Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead)
{
    HRESULT hr = S_OK;
    ULONG cbBytesRead = 0;
    BYTE* pBuffer = (BYTE*)_memory.data();

    // Adjust for set memory pointer
    if (_memory.empty())
    {
        pBuffer = (BYTE*)_pMemory;
    }

    ULONGLONG cbStreamTotalBytes = GetStreamTotalSizeBytes();
    ULONGLONG cbStreamBytesAvailableToRead = (cbStreamTotalBytes - _CurrentPosition.QuadPart);

    // Ensure that the current buffer pointer is accessing the memory from the
    // current location.
    pBuffer = pBuffer + _CurrentPosition.QuadPart;
        
    // If there is data available to read, attempt to read it...
    if ((cbStreamBytesAvailableToRead > 0) && (cb > 0))
    {
        if (cb <= cbStreamBytesAvailableToRead)
        {
            // requested buffer size is less or equal to the remaining data size
            cbBytesRead = cb;
        }
        else
        {
            // requested buffer is larger than remaining data size
            // Read available memory and return.
            cbBytesRead = (ULONG)cbStreamBytesAvailableToRead;
            hr = S_FALSE; // read past the end of the stream
        }
        
        memcpy(pv, pBuffer, cbBytesRead);
    }
    
    // Update the current position
    _CurrentPosition.QuadPart += cbBytesRead;

    if (pcbRead)
    {
        *pcbRead = cbBytesRead;
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten)
{
    if (_pMemory != nullptr)
    {
        return E_NOTIMPL;
    }

    HRESULT hr = S_OK;
    BYTE* pBuffer = (BYTE*)_memory.data();
    ULONGLONG cbStreamTotalBytes = GetStreamTotalSizeBytes();
    ULONGLONG cbStreamBytesAvailableToWrite = 0;

    if (_CurrentPosition.QuadPart >= cbStreamTotalBytes)
    {
        // Stream has been seeked past the actual data and a write is occuring. This means that
        // the actual backing memory needs to be reallocated.
        cbStreamBytesAvailableToWrite = (_CurrentPosition.QuadPart - cbStreamTotalBytes) + cb;
        ReAllocMemory((ULONG)cbStreamBytesAvailableToWrite);
    }
    else
    {
        cbStreamBytesAvailableToWrite = (cbStreamTotalBytes - _CurrentPosition.QuadPart);
        // If the write request is larger than the available memory
        // realloc to contain the new data.
        if (cb > cbStreamBytesAvailableToWrite)
        {
            ReAllocMemory((ULONG)(cb - cbStreamBytesAvailableToWrite));
        }
    }

    // Ensure that the current buffer pointer is accessing the memory from the
    // current location.
    pBuffer = (BYTE*)_memory.data();
    pBuffer = pBuffer + _CurrentPosition.QuadPart;
                    
    memcpy(pBuffer, pv, cb);
    
    // Update the current position
    _CurrentPosition.QuadPart += cb;

    if (pcbWritten)
    {
        *pcbWritten = cb;
    }

    return hr;
}

// IStream
HRESULT STDMETHODCALLTYPE MemoryStream::SetSize(_In_ ULARGE_INTEGER libNewSize)
{
    UNREFERENCED_PARAMETER(libNewSize);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::CopyTo(_In_ IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten)
{
    UNREFERENCED_PARAMETER(pStream);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(pcbRead);
    UNREFERENCED_PARAMETER(pcbWritten);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Commit(_In_ DWORD grfCommitFlags)
{
    UNREFERENCED_PARAMETER(grfCommitFlags);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Revert()
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Clone(_Deref_out_ IStream **ppStream)
{
    UNREFERENCED_PARAMETER(ppStream);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* plibNewPosition)
{
    HRESULT hr = S_OK;

    // Always report the current position.  This ensure that if an error were to occur the returned position is always
    // correct.
    if (plibNewPosition)
    {
        *plibNewPosition = _CurrentPosition;
    }

    switch (dwOrigin)
    {
    case STREAM_SEEK_SET:
        {
            if (liDistanceToMove.QuadPart >= 0)
            {
                // Set the current position
                _CurrentPosition.QuadPart = liDistanceToMove.QuadPart;
            }
            else
            {
                hr = STG_E_SEEKERROR;
            }
        }
        break;
    case STREAM_SEEK_CUR:
        {
            // Set the current position
            if ((liDistanceToMove.QuadPart < 0) && ((ULONGLONG)_abs64(liDistanceToMove.QuadPart) > _CurrentPosition.QuadPart))
            {
                hr = STG_E_SEEKERROR;
            }

            if (SUCCEEDED(hr))
            {
                _CurrentPosition.QuadPart += liDistanceToMove.QuadPart;
            }
        }
        break;
    default:
        hr = E_INVALIDARG;
        break;
    }

    if (plibNewPosition)
    {
        *plibNewPosition = _CurrentPosition;
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE MemoryStream::Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag)
{
    if (pStatstg == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    switch(grfStatFlag)
    {
    case STGTY_STORAGE:
    case STGTY_STREAM:
        {
            memset(pStatstg, 0, sizeof(STATSTG));
            pStatstg->type = grfStatFlag;
            pStatstg->cbSize.QuadPart = GetStreamTotalSizeBytes();
            hr = S_OK;
        }
        break;
    case STGTY_LOCKBYTES:
    case STGTY_PROPERTY:
    default:
        hr = E_INVALIDARG;
        break;
    }

    return S_OK;
}

ULONGLONG MemoryStream::GetStreamTotalSizeBytes()
{
    if (_pMemory != nullptr)
    {
        return _cbMemory;
    }
    else
    {
        return (ULONGLONG)_memory.size();
    }
}

HRESULT MemoryStream::ReAllocMemory(ULONG cb)
{
    if (_pMemory != nullptr)
    {
        return E_UNEXPECTED;
    }

    _memory.resize(GetStreamTotalSizeBytes() + cb);

    return S_OK;
}
