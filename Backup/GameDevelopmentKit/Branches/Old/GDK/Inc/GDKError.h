#pragma once

namespace GDK
{
    class Exception
    {
    public:
        // Construction / destruction
        Exception(_In_ HRESULT hr, _In_opt_z_ const wchar_t* codeSnippet, _In_opt_z_ const wchar_t* filename, _In_ int32_t lineNumber) :
            _hr(hr), 
            _message(nullptr)
        {
#ifdef WIN32
            static wchar_t sharedBuffer[1000];
            wchar_t* formattedError = nullptr;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                (LPWSTR)&formattedError, 0, nullptr);

            if (formattedError)
            {
                formattedError[wcslen(formattedError) - 2] = 0;
            }

            wsprintf(sharedBuffer, L"Error: %s failed with 0x%08x (%s) (%s: %d)", codeSnippet ? codeSnippet : L"", 
                hr, formattedError, filename ? filename : L"",  lineNumber);

            if (formattedError)
            {
                LocalFree(formattedError);
            }

            size_t length = wcslen(sharedBuffer);
            _message = new wchar_t[length + 1];
            wcscpy_s(_message, length + 1, sharedBuffer);
            _message[length] = L'\0';
#endif
        }

        virtual ~Exception()
        {
            SafeDeleteArray(_message);
        }

        // Accessors
        HRESULT GetResult() const { return _hr; }
        const wchar_t* GetMessage() const { return _message; }

    private:
        HRESULT _hr;
        wchar_t* _message;
    };

// Handle HRESULT style errors
#define CHECKHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) throw GDK::Exception(_hr, TXT(x), __FILEW__, __LINE__); }

// Handle WIN32 style errors
#define CHECKW32(x) { int32_t _result = (x); if (_result != ERROR_SUCCESS) throw GDK::Exception(HRESULT_FROM_WIN32(_result), TXT(x), __FILEW__, __LINE__); }

// Custom throws
#define GDK_THROW(hr, x) { throw GDK::Exception(hr, TXT(x), __FILEW__, __LINE__); }

// Handle NULL checks
#define CHECK_NOT_NULL(x, hr) { if (!(x)) throw GDK::Exception((hr), TXT(x), __FILEW__, __LINE__); }

// Handle boolean checks
#define CHECK_IS_TRUE(x, hr) { if (!(x)) throw GDK::Exception((hr), TXT(x), __FILEW__, __LINE__); }

//
// Module boundary guards
//
#define MODULE_GUARD_BEGIN try { 
#define MODULE_GUARD_END } catch (const GDK::Exception& ex) { DebugOut(L"%s\n", ex.GetMessage()); return ex.GetResult(); } catch (const std::exception& ex2) { DebugOut(L"%S\n", ex2.what()); return E_FAIL; } return S_OK;

} // GDK
