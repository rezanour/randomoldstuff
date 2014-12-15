#include "SubStream.h"
#include "Debug.h"

using namespace CoreServices;

SubStream::SubStream(_In_ IStream* pStream, _In_ size_t length) :
    _refcount(1),
    _spStream(pStream),
    _cbMemory(length)
{
    _CurrentPosition.QuadPart = 0;
    if (_spStream != nullptr)
    {
        LARGE_INTEGER liMove = {0};
        _spStream->Seek(liMove, STREAM_SEEK_CUR, &_backingStreamCurrentPosition);
    }
}

SubStream::~SubStream()
{
    
}

// Static methods
HRESULT SubStream::Create(_In_ IStream* pStream, _In_ size_t length, _Deref_out_ IStream** ppStream)
{
    if (ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppStream = nullptr;

    HRESULT hr = S_OK;

    // If the passed in stream and the specified restricted length is too large
    // fail creation of the substream
    STATSTG stat = {0};
    LARGE_INTEGER liMove = {0};
    ULARGE_INTEGER uliPos = {0};

    hr = pStream->Stat(&stat, STGTY_STREAM);
    if (SUCCEEDED(hr))
    {
        // Get current position in the stream
        hr = pStream->Seek(liMove, STREAM_SEEK_CUR, &uliPos);
        if (SUCCEEDED(hr))
        {
            if ((stat.cbSize.QuadPart - uliPos.QuadPart) < length)
            {
                return E_INVALIDARG;
            }
        }
    }

    *ppStream = new SubStream(pStream, length);
    if(*ppStream == NULL)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT SubStream::Seek(_In_ IStream* pStream, DWORD dwOrigin, LONG offset)
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
HRESULT STDMETHODCALLTYPE SubStream::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE SubStream::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE SubStream::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

// ISequentialStream
HRESULT STDMETHODCALLTYPE SubStream::Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead)
{
    HRESULT hr = S_OK;
    ULONG cbBytesRead = 0;
    
    ULONGLONG cbStreamTotalBytes = GetStreamTotalSizeBytes();
    ULONGLONG cbStreamBytesAvailableToRead = (cbStreamTotalBytes - _CurrentPosition.QuadPart);

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
        
        // If we fail for some reason to read from the backing stream, report to the
        // caller an accurate reason and overrite any previously set HRESULT
        HRESULT Temphr = _spStream->Read(pv, cb, &cbBytesRead);
        if FAILED(Temphr)
        {
            hr = Temphr;
        }
    }
    
    // Update the current position
    _CurrentPosition.QuadPart += cbBytesRead;

    if (pcbRead)
    {
        *pcbRead = cbBytesRead;
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE SubStream::Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten)
{
    HRESULT hr = S_OK;
    ULONG cbWritten = 0;
    hr = _spStream->Write(pv, cb, &cbWritten);
    
    ULONGLONG cbStreamTotalBytes = GetStreamTotalSizeBytes();
    ULONGLONG cbStreamBytesAvailableToWrite = 0;

    if (_CurrentPosition.QuadPart >= cbStreamTotalBytes)
    {
        // Stream has been seeked past the actual data and a write is occuring.
        cbStreamBytesAvailableToWrite = (_CurrentPosition.QuadPart - cbStreamTotalBytes) + cb;
        hr = E_FAIL; 
    }
    else
    {
        cbStreamBytesAvailableToWrite = (cbStreamTotalBytes - _CurrentPosition.QuadPart);
        // If the write request is larger than the available memory
        if (cb > cbStreamBytesAvailableToWrite)
        {
            hr = S_FALSE;
        }
    }

    if (SUCCEEDED(hr))
    {
        HRESULT Temphr = _spStream->Write(pv, cb, pcbWritten);
        if (FAILED(Temphr))
        {
            hr = Temphr;
        }
    }
    
    if (SUCCEEDED(hr))
    {
        // Update the current position
        _CurrentPosition.QuadPart += cb;

        if (pcbWritten)
        {
            *pcbWritten = cb;
        }
    }

    return hr;
}

// IStream
HRESULT STDMETHODCALLTYPE SubStream::SetSize(_In_ ULARGE_INTEGER libNewSize)
{
    UNREFERENCED_PARAMETER(libNewSize);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::CopyTo(_In_ IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten)
{
    UNREFERENCED_PARAMETER(pStream);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(pcbRead);
    UNREFERENCED_PARAMETER(pcbWritten);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::Commit(_In_ DWORD grfCommitFlags)
{
    UNREFERENCED_PARAMETER(grfCommitFlags);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::Revert()
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
{
    UNREFERENCED_PARAMETER(libOffset);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(dwLockType);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::Clone(_Deref_out_ IStream **ppStream)
{
    UNREFERENCED_PARAMETER(ppStream);

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SubStream::Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* plibNewPosition)
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

                // Perform adjusted seek against backing stream
                LARGE_INTEGER adjustedSeekPosition;
                adjustedSeekPosition.QuadPart = _backingStreamCurrentPosition.QuadPart + liDistanceToMove.QuadPart;
                hr = _spStream->Seek(adjustedSeekPosition, dwOrigin, nullptr);
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
                hr = _spStream->Seek(liDistanceToMove, dwOrigin, nullptr);
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

HRESULT STDMETHODCALLTYPE SubStream::Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag)
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

ULONGLONG SubStream::GetStreamTotalSizeBytes()
{
    if (_spStream != nullptr)
    {
        return _cbMemory;
    }
    else
    {
        return 0;
    }
}

