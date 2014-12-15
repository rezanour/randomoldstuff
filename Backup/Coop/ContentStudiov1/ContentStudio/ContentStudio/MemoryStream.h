#pragma once

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(x) \
   if((x != NULL) && (x != INVALID_HANDLE_VALUE)) \
   {                                              \
      ::CloseHandle(x);                           \
      x = NULL;                                   \
   }
#endif

class MemoryStream : public IStream
{
    MemoryStream(void* pMemory, size_t cbMemory) :
        _refcount(1),
        _hMemory(NULL),
        _bDeleteOnRelease(FALSE),
        _pMemory(pMemory),
        _cbMemory(cbMemory)
    {
        _CurrentPosition.QuadPart = 0;
        
        memset(&_CurrentPosition, 0, sizeof(_CurrentPosition));
    }

    MemoryStream(HGLOBAL hMemory, BOOL bDeleteOnRelease) :
        _refcount(1),
        _hMemory(hMemory),
        _bDeleteOnRelease(bDeleteOnRelease),
        _pMemory(NULL),
        _cbMemory(0)
    {
        _CurrentPosition.QuadPart = 0;
        
        memset(&_CurrentPosition, 0, sizeof(_CurrentPosition));

        if (hMemory)
        {
            _hMemory = hMemory;
        }
        else
        {
            _hMemory = GlobalAlloc(GPTR, 0);
        }
    }

    virtual ~MemoryStream() 
    { 
        if ((_hMemory) && (_bDeleteOnRelease))
        {
            GlobalFree(_hMemory);
            _hMemory = NULL;
        }
    }

public:
    HRESULT static OpenStream(HGLOBAL hMemory, bool bDeleteOnRelease, IStream ** ppStream)
    {
        *ppStream = new MemoryStream(hMemory, bDeleteOnRelease);
            
        return S_OK;
    }

    HRESULT static AttachStream(void* pMemory, size_t cbMemory, IStream** ppStream)
    {
        *ppStream = new MemoryStream(pMemory, cbMemory);
        return S_OK;
    }

    HRESULT static AttachStream(HGLOBAL hMemory, IStream ** ppStream)
    {
        *ppStream = new MemoryStream(hMemory, false);
        return S_OK;
    }

    HRESULT static SeekStream(IStream* pStream, DWORD dwOrigin, LONG offset)
    {
        HRESULT hr = S_OK;

        LARGE_INTEGER liMove = {0};
        ULARGE_INTEGER uliPos = {0};
        liMove.LowPart = offset;
        
        hr = pStream->Seek(liMove, dwOrigin, &uliPos);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking stream");

    Exit:

        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject)
    { 
        if (iid == __uuidof(IUnknown)
            || iid == __uuidof(IStream)
            || iid == __uuidof(ISequentialStream))
        {
            *ppvObject = static_cast<IStream*>(this);
            AddRef();
            return S_OK;
        } 
        
        return E_NOINTERFACE; 
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void) 
    { 
        return (ULONG)InterlockedIncrement(&_refcount); 
    }

    virtual ULONG STDMETHODCALLTYPE Release(void) 
    {
        ULONG res = (ULONG) InterlockedDecrement(&_refcount);
        if (res == 0) 
        {
            delete this;
        }

        return res;
    }

    // ISequentialStream Interface
public:
    virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead)
    {
        //DEBUG_PRINT("MemoryStream::Read(%d bytes) at position(%d)", cb, _CurrentPosition.QuadPart);

        HRESULT hr = S_OK;
        ULONG cbBytesRead = 0;
        BYTE* pBuffer = (BYTE*)_hMemory;

        // Adjust for set memory pointer
        if (_hMemory == NULL)
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

    virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten)
    {
        //DEBUG_PRINT("MemoryStream::Write(%d bytes) at position(%d)", cb, _CurrentPosition.QuadPart);

        if (_pMemory != NULL)
        {
            return E_NOTIMPL;
        }

        HRESULT hr = S_OK;
        BYTE* pBuffer = (BYTE*)_hMemory;
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
        pBuffer = (BYTE*)_hMemory;
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

    // IStream Interface
public:
    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*)  { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE Revert(void)  { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE Clone(IStream **) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
    { 
        //DEBUG_PRINT("MemoryStream::Seek[BEGIN](Position before seek %d)", _CurrentPosition.QuadPart);

        HRESULT hr = S_OK;
        
        // Always report the current position.  This ensure that if an error were to occur the returne position is always
        // correct.
        if (plibNewPosition)
        {
            *plibNewPosition = _CurrentPosition;
        }

        switch (dwOrigin)
        {
        case STREAM_SEEK_SET:
            {
                //DEBUG_PRINT("MemoryStream::Seek(%d, STREAM_SEEK_SET)", dlibMove.QuadPart);
                if (dlibMove.QuadPart >= 0)
                {
                    // Set the current position
                    _CurrentPosition.QuadPart = dlibMove.QuadPart;            
                }
                else
                {
                    hr = STG_E_SEEKERROR;
                }
            }
            break;
        case STREAM_SEEK_CUR:
            {
                //DEBUG_PRINT("MemoryStream::Seek(%d, STREAM_SEEK_CUR)", dlibMove.QuadPart);

                // Set the current position
                if ((dlibMove.QuadPart < 0) && ((ULONGLONG)_abs64(dlibMove.QuadPart) > _CurrentPosition.QuadPart))
                {
                    hr = STG_E_SEEKERROR;
                }

                if (SUCCEEDED(hr))
                {
                    _CurrentPosition.QuadPart += dlibMove.QuadPart;
                }
            }
            break;
        default:
            //DEBUG_PRINT("MemoryStream::Seek(INVALID)");
            hr = E_INVALIDARG;
            break;
        }


        if (plibNewPosition)
        {
            *plibNewPosition = _CurrentPosition;
        }

        //DEBUG_PRINT("MemoryStream::Seek[END](Position after seek %d)", _CurrentPosition.QuadPart);
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag) 
    {
        HRESULT hr = E_INVALIDARG;
        if (!pStatstg)
        {
            return E_INVALIDARG;
        }

        switch(grfStatFlag)
        {
        case STGTY_STORAGE:
        case STGTY_STREAM:
            {
                memset(pStatstg, 0, sizeof(STATSTG));
                pStatstg->type = grfStatFlag;
                pStatstg->cbSize.QuadPart = GetStreamTotalSizeBytes();
                //DEBUG_PRINT("MemoryStream::Stat(%d bytes reported)", pStatstg->cbSize.QuadPart);
                hr = S_OK;
            }
            break;    
        case STGTY_LOCKBYTES:
        case STGTY_PROPERTY:    
        default:
            //DEBUG_PRINT("MemoryStream::Stat(INVALID - %d)", grfStatFlag);
            break;
        }
        return hr;
    }

private:
    ULONGLONG GetStreamTotalSizeBytes()
    {
        if (_pMemory != NULL)
        {
            return _cbMemory;
        }
        else
        {
            return (ULONGLONG)GlobalSize(_hMemory);
        }
    }

    HRESULT ReAllocMemory(ULONG cb)
    {
        //DEBUG_PRINT("MemoryStream::ReAllocMemory(%d bytes)", cb);

        if (_pMemory != NULL)
        {
            return E_UNEXPECTED;
        }

        HRESULT hr = S_OK;
        ULONGLONG cbCurrentStreamSize = GetStreamTotalSizeBytes();
        HGLOBAL hMemory = GlobalAlloc(GPTR, (ULONG)(cb + cbCurrentStreamSize));
        BYTE* pNewMemory = (BYTE*)hMemory;
        BYTE* pOldMemory = (BYTE*)_hMemory;

        memcpy(pNewMemory, pOldMemory, (ULONG)cbCurrentStreamSize);

        GlobalFree(_hMemory);
        _hMemory = hMemory;

        return hr;
    }

private:
    void* _pMemory;
    size_t _cbMemory;
    HGLOBAL _hMemory;
    BOOL _bDeleteOnRelease;
    ULARGE_INTEGER _CurrentPosition;
    LONG _refcount;
};
