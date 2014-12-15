#include "Precomp.h"
#include <GDKStream.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class GDKFileStream : public RuntimeObject<IStream>
    {
    public:
        static HRESULT GDKAPI Create(_In_z_ const wchar_t* path, bool readOnly, _COM_Outptr_ IStream** stream)
        {
            MODULE_GUARD_BEGIN
            
            CHECK_NOT_NULL(stream, E_POINTER);
            CHECK_NOT_NULL(path, E_INVALIDARG);
            if (readOnly)
            {
                CHECK_IS_TRUE((GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES), E_INVALIDARG);
            }
            *stream = nullptr;

            HANDLE file = INVALID_HANDLE_VALUE;
            file = ::CreateFileW(path, readOnly ? GENERIC_READ : GENERIC_WRITE, readOnly ? FILE_SHARE_READ : FILE_SHARE_WRITE, nullptr, readOnly ? OPEN_EXISTING : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file != INVALID_HANDLE_VALUE)
            {
                ComPtr<GDKFileStream> str = Make<GDKFileStream>(file, path);
                CHECKHR(str.CopyTo(stream));
            }
            else
            {
                CHECKHR(HRESULT_FROM_WIN32(::GetLastError()));
            }
           
            MODULE_GUARD_END
        }

        GDKFileStream(HANDLE file,
            const wchar_t* name) :
            _file(file),
            _attached(false)
        {
            if (name)
            {
                _name = name;
            }
            else
            {
                _name = L"unnamed";
            }
        }

    public:
        // Extended GDK IStream
        HRESULT STDMETHODCALLTYPE SeekTo(DWORD dwOrigin, LONG offset)
        {            
            LARGE_INTEGER liMove = {0};
            ULARGE_INTEGER uliPos = {0};
            liMove.LowPart = offset;

            return Seek(liMove, dwOrigin, &uliPos);
        }

        HRESULT STDMETHODCALLTYPE GetName(_Out_ const wchar_t** name)
        {
            if (!name)
            {
                return E_INVALIDARG;
            }

            *name = _name.c_str();

            return S_OK;
        }
                
        // ISequentialStream
        HRESULT STDMETHODCALLTYPE Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead)
        {
            BOOL rc = ReadFile(_file, pv, cb, pcbRead, nullptr);
            return (rc) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }

        HRESULT STDMETHODCALLTYPE Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten)
        {
            BOOL rc = WriteFile(_file, pv, cb, pcbWritten, nullptr);
            return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }

        // IStream
        HRESULT STDMETHODCALLTYPE SetSize(_In_ ULARGE_INTEGER libNewSize)
        {
            UNREFERENCED_PARAMETER(libNewSize);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CopyTo(_In_ ::IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten)
        {
            UNREFERENCED_PARAMETER(pStream);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(pcbRead);
            UNREFERENCED_PARAMETER(pcbWritten);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Commit(_In_ DWORD grfCommitFlags)
        {
            UNREFERENCED_PARAMETER(grfCommitFlags);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Revert()
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
        {
            UNREFERENCED_PARAMETER(libOffset);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(dwLockType);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
        {
            UNREFERENCED_PARAMETER(libOffset);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(dwLockType);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Clone(_Deref_out_ ::IStream **ppStream)
        {
            UNREFERENCED_PARAMETER(ppStream);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* lpNewFilePointer)
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

        HRESULT STDMETHODCALLTYPE Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag)
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


    private:
        ~GDKFileStream()
        {
            if (_file != INVALID_HANDLE_VALUE)
            {
                CloseHandle(_file);
            }
        }

        HANDLE _file;
        bool _attached;
        std::wstring _name;
    };
    
    // Memory Stream
    class GDKMemoryStream : public RuntimeObject<IStream>
    {
    public:
        static HRESULT GDKAPI Create(_In_ size_t length, _COM_Outptr_ IStream** stream)
        {
            MODULE_GUARD_BEGIN
            
            CHECK_NOT_NULL(stream, E_POINTER);
            *stream = nullptr;

            ComPtr<GDKMemoryStream> str = Make<GDKMemoryStream>(length, L"memory stream");
            CHECKHR(str.CopyTo(stream));
                       
            MODULE_GUARD_END
        }

        GDKMemoryStream(size_t length,
            const wchar_t* name) :
            _pMemory(nullptr),
            _cbMemory(0)
        {
            _CurrentPosition.QuadPart = 0;
            _memory.resize(length);

            if (name)
            {
                _name = name;
            }
            else
            {
                _name = L"unnamed";
            }
        }

    public:
        // Extended GDK IStream
        HRESULT STDMETHODCALLTYPE SeekTo(DWORD dwOrigin, LONG offset)
        {            
            LARGE_INTEGER liMove = {0};
            ULARGE_INTEGER uliPos = {0};
            liMove.LowPart = offset;

            return Seek(liMove, dwOrigin, &uliPos);
        }

        HRESULT STDMETHODCALLTYPE GetName(_Out_ const wchar_t** name)
        {
            if (!name)
            {
                return E_INVALIDARG;
            }

            *name = _name.c_str();

            return S_OK;
        }
                
        // ISequentialStream
        HRESULT STDMETHODCALLTYPE Read(_In_ void* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbRead)
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

        HRESULT STDMETHODCALLTYPE Write(_In_ void const* pv, _In_ ULONG cb, _In_opt_ ULONG* pcbWritten)
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
        HRESULT STDMETHODCALLTYPE SetSize(_In_ ULARGE_INTEGER libNewSize)
        {
            UNREFERENCED_PARAMETER(libNewSize);
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CopyTo(_In_ ::IStream* pStream, _In_ ULARGE_INTEGER cb, _In_opt_ ULARGE_INTEGER* pcbRead, _In_opt_ ULARGE_INTEGER* pcbWritten)
        {
            UNREFERENCED_PARAMETER(pStream);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(pcbRead);
            UNREFERENCED_PARAMETER(pcbWritten);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Commit(_In_ DWORD grfCommitFlags)
        {
            UNREFERENCED_PARAMETER(grfCommitFlags);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Revert()
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE LockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
        {
            UNREFERENCED_PARAMETER(libOffset);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(dwLockType);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE UnlockRegion(_In_ ULARGE_INTEGER libOffset, _In_ ULARGE_INTEGER cb, _In_ DWORD dwLockType)
        {
            UNREFERENCED_PARAMETER(libOffset);
            UNREFERENCED_PARAMETER(cb);
            UNREFERENCED_PARAMETER(dwLockType);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Clone(_Deref_out_ ::IStream **ppStream)
        {
            UNREFERENCED_PARAMETER(ppStream);

            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE Seek(_In_ LARGE_INTEGER liDistanceToMove, _In_ DWORD dwOrigin, _In_opt_ ULARGE_INTEGER* plibNewPosition)
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

        HRESULT STDMETHODCALLTYPE Stat(_In_ STATSTG* pStatstg, _In_ DWORD grfStatFlag)
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

        ULONGLONG GetStreamTotalSizeBytes()
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

        HRESULT ReAllocMemory(ULONG cb)
        {
            if (_pMemory != nullptr)
            {
                return E_UNEXPECTED;
            }

            _memory.resize(GetStreamTotalSizeBytes() + cb);

            return S_OK;
        }

    private:
        ~GDKMemoryStream()
        {
        }

        std::vector<byte> _memory;
        ULARGE_INTEGER _CurrentPosition;
        void* _pMemory;
        size_t _cbMemory;
        std::wstring _name;
    };

    extern "C" HRESULT CreateFileStream(_In_z_ const wchar_t* path, bool readOnly, _COM_Outptr_ IStream** stream)
    {
        return GDKFileStream::Create(path, readOnly, stream);
    }

    extern "C" HRESULT CreateMemoryStream(_In_ size_t length, _COM_Outptr_ IStream** stream)
    {
        return GDKMemoryStream::Create(length, stream);
    }
}