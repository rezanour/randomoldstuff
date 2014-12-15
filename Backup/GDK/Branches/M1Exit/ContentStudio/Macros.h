#pragma once

#ifndef DEBUG_PRINT
#ifdef _DEBUG
    #define DEBUG_PRINT DebugOut
#else
    #define DEBUG_PRINT
#endif
#endif

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(x) \
   if((x != NULL) && (x != INVALID_HANDLE_VALUE)) \
   {                                              \
      ::CloseHandle(x);                           \
      x = NULL;                                   \
   }
#endif

#ifndef SAFE_FREELIBRARY
#define SAFE_FREELIBRARY(x) \
   if((x != NULL) && (x != INVALID_HANDLE_VALUE)) \
   {                                              \
      ::FreeLibrary(x);                           \
      x = NULL;                                   \
   }
#endif

#ifndef SAFE_ADDREF
#define SAFE_ADDREF(x)  \
   if(x != NULL)        \
   {                    \
      x->AddRef();      \
      x = NULL;         \
   }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
   if(x != NULL)        \
   {                    \
      x->Release();     \
      x = NULL;         \
   }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
   if(x != NULL)       \
   {                   \
      delete x;        \
      x = NULL;        \
   }
#endif

#ifndef SAFE_DELETE_NONULL
#define SAFE_DELETE_NONULL(x) \
   if(x != NULL)       \
   {                   \
      delete x;        \
   }
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) \
   if(x != NULL)             \
   {                         \
      delete[] x;            \
      x = NULL;              \
   }
#endif

#ifndef GOTO_EXIT_IF_FAILED
#define GOTO_EXIT_IF_FAILED(hr) if(FAILED(hr)) goto Exit
#endif

#ifndef GOTO_EXIT_IF_FAILED_MESSAGE
#define GOTO_EXIT_IF_FAILED_MESSAGE(hr, s)         \
    if(FAILED(hr))                                 \
    {                                              \
        dxutodprintf("%s failed, hr = 0x%lx",s,hr);    \
        goto Exit;                                 \
    }                                              \
    else if(hr == S_FALSE)                         \
    {                                              \
        dxutodprintf("%s, S_FALSE returned", s);       \
    }
#endif

// ConvertTo usage
//
// std::string str1 = ConvertTo<std::string>(35);
// std::string str2 = ConvertTo<std::string>(4.5);
// double d = ConvertTo<double>("45.67");
//
template<typename TRes, typename TArg>
TRes ConvertTo(const TArg &rArg)
{
    std::wstringstream ss;
    ss << rArg;
    TRes res = TRes();
    ss >> res;
    return res;
}

template<typename TRes, typename TArg>
TRes ConvertToA(const TArg &rArg)
{
    std::stringstream ss;
    ss << rArg;
    TRes res = TRes();
    ss >> res;
    return res;
}

template <typename TArg>
struct stdArrayDeleter
{
    void operator ()(TArg* p)
    {
        delete [] p;
    }
};

