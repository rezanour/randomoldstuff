#pragma once

// debug output
#pragma warning( disable : 4996 ) // disable deprecated warning 
void __cdecl plugindprintf(const char *format, ...);

#ifndef DEBUG_PRINT
#ifdef _DEBUG
    #define DEBUG_PRINT plugindprintf
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

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) \
   if(x != NULL)       \
   {                   \
      delete [] x;     \
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
        plugindprintf("%s failed, hr = 0x%lx",s,hr);    \
        goto Exit;                                 \
    }                                              \
    else if(hr == S_FALSE)                         \
    {                                              \
        plugindprintf("%s, S_FALSE returned", s);       \
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

struct XMPosTexNormTanBiNormVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT2 texCoord; // Texture coordinate
    XMFLOAT3 normal;   // Normal
    XMFLOAT3 tanget;   // Tanget
    XMFLOAT3 binormal; // BiNormal
};

struct XMPosTexNormVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT2 texCoord; // Texture coordinate
    XMFLOAT3 normal; // Normal
};

struct XMPosTexNormVertexId
{
    XMPosTexNormVertex vertex;
    UINT vertexId;
};

struct XMPosNormTexVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT3 normal; // Normal
    XMFLOAT2 texCoord; // Texture coordinate
};

struct XMPosNormTanBiTexVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT3 normal; // Normal
    XMFLOAT3 tangent; // Tangent
    XMFLOAT3 biTangent; // BiTangent
    XMFLOAT2 texCoord; // Texture coordinate
};

void TriangulateFace(std::vector<XMPosTexNormVertexId>& face, bool swapWindingOrder);
void ScaleVertices(XMPosNormTexVertex* vertices, UINT numVertices, float scale);
void NormalizeVector3(XMFLOAT3& vector3);
void NormalizeVector2(XMFLOAT2& vector2);
XMFLOAT3 CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);