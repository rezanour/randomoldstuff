#pragma once

#include <Platform.h>
#include <NonCopyable.h>

namespace GDK
{
    class GDKException : public std::exception
    {
    public:
        GDKException(_In_opt_z_ const char* source, _In_z_ const char* filename, _In_ int32_t lineNumber, _In_ int32_t errorCode) :
            _errorCode(errorCode)
        {
            static char message[500] = {0};

            sprintf_s(message, "%s%s0x%08x (%d) [%s: %d]", 
                source ? source : "", 
                source ? " failed with " : "", 
                errorCode, errorCode, 
                filename, 
                lineNumber);

            _message.assign(message);
        }

        virtual const char* what() const throw() override
        {
            return _message.c_str();
        }

        int32_t GetErrorCode() const
        {
            return _errorCode;
        }

    private:
        GDKException();

        std::string _message;
        int32_t _errorCode;
    };

#if GDKDEBUG
#define TOKENIZE_SOURCE(x) #x
#else
#define TOKENIZE_SOURCE(x) nullptr
#endif

#define CHECK_HR(x)                 { HRESULT hr = (x); if (FAILED(hr)) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, static_cast<int32_t>(hr)); }
#define CHECK_WIN32(x)              if (!(x)) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, static_cast<int32_t>(GetLastError()));
#define CHECK_NOT_NULL(x)           if ((x) == nullptr) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, 0);
#define CHECK_RANGE(x, min, max)    { auto temp_v = (x); if (temp_v < min || temp_v > max) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, 0); }
#define CHECK_TRUE(x)               if (!(x)) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, 0);
#define CHECK_FALSE(x)              if (x) throw GDK::GDKException(TOKENIZE_SOURCE(x), __FILE__, __LINE__, 0);
#define RAISE_EXCEPTION(msg, code)  throw GDK::GDKException(msg, __FILE__, __LINE__, code);
    
}
