#include <windows.h>
#include <Utility.h>
#include <assert.h>

#define SIGNATURE '8==D'

typedef struct
{
    uint32_t Signature;
    float Mins[3];
    float Maxs[3];

    AabbNode* Node;
} GameObject;

void GetGameObjectBounds(void* object, float mins[3], float maxs[3])
{
    GameObject* gameObject = (GameObject*)object;

    assert(gameObject->Signature == SIGNATURE);

    memcpy(mins, gameObject->Mins, sizeof(gameObject->Mins));
    memcpy(maxs, gameObject->Maxs, sizeof(gameObject->Maxs));
}

uint32_t buffer[10000];
long next = 0;

DWORD CALLBACK ThreadProcSpinLock(void* p)
{
    UMSpinLock* lock = (UMSpinLock*)p;

    long value = InterlockedIncrement(&next);

    UMSpinLockAcquire(lock);

    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        buffer[i] = value;
        YieldProcessor();
    }

    UMSpinLockRelease(lock);

    return 0;
}

DWORD CALLBACK ThreadProcCS(void* p)
{
    UMCriticalSection* lock = (UMCriticalSection*)p;

    long value = InterlockedIncrement(&next);

    UMCriticalSectionAcquire(lock);

    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        buffer[i] = value;
        YieldProcessor();
    }

    UMCriticalSectionRelease(lock);

    return 0;
}

DWORD CALLBACK ThreadProcOSCS(void* p)
{
    CRITICAL_SECTION* lock = (CRITICAL_SECTION*)p;

    long value = InterlockedIncrement(&next);

    EnterCriticalSection(lock);

    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        buffer[i] = value;
        YieldProcessor();
    }

    LeaveCriticalSection(lock);

    return 0;
}

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int commandShow)
{
    UMSpinLock spinLock;
    UMCriticalSection cs;
    CRITICAL_SECTION cs2;
    LARGE_INTEGER freq, start, stop, osCS, myCS;

    HANDLE threads[64];

    UMSpinLockInitialize(&spinLock);
    UMCriticalSectionInitialize(&cs, _countof(threads));
    InitializeCriticalSection(&cs2);

    for (uint32_t i = 0; i < _countof(threads); ++i)
    {
        threads[i] = CreateThread(NULL, 0, ThreadProcSpinLock, &spinLock, 0, NULL);
        assert(threads[i] != NULL);
    }

    WaitForMultipleObjects(_countof(threads), threads, TRUE, INFINITE);
    uint32_t value = buffer[0];
    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        if (buffer[i] != value)
        {
            OutputDebugString(L"\n\nERROR ERROR\n");
            assert(false);
        }
    }

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    for (uint32_t i = 0; i < _countof(threads); ++i)
    {
        CloseHandle(threads[i]);
        threads[i] = CreateThread(NULL, 0, ThreadProcCS, &cs, 0, NULL);
        assert(threads[i] != NULL);
    }

    WaitForMultipleObjects(_countof(threads), threads, TRUE, INFINITE);
    QueryPerformanceCounter(&stop);
    myCS.QuadPart = stop.QuadPart - start.QuadPart;

    value = buffer[0];
    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        if (buffer[i] != value)
        {
            OutputDebugString(L"\n\nERROR ERROR\n");
            assert(false);
        }
    }

    QueryPerformanceCounter(&start);
    for (uint32_t i = 0; i < _countof(threads); ++i)
    {
        CloseHandle(threads[i]);
        threads[i] = CreateThread(NULL, 0, ThreadProcOSCS, &cs2, 0, NULL);
        assert(threads[i] != NULL);
    }

    WaitForMultipleObjects(_countof(threads), threads, TRUE, INFINITE);
    QueryPerformanceCounter(&stop);
    osCS.QuadPart = stop.QuadPart - start.QuadPart;

    value = buffer[0];
    for (uint32_t i = 0; i < _countof(buffer); ++i)
    {
        if (buffer[i] != value)
        {
            OutputDebugString(L"\n\nERROR ERROR\n");
            assert(false);
        }
    }

    for (uint32_t i = 0; i < _countof(threads); ++i)
    {
        CloseHandle(threads[i]);
    }

    UMCriticalSectionDestroy(&cs);

    typedef struct
    {
        uint32_t X;
        float F;
    } Foo;

    Foo f;
    Foo* g;
    Handle h;
    char buffer[] = "Hello, World";
    wchar_t buffer2[100];
    char buffer3[100];

    GameObject go1;
    GameObject go2;
    AabbNode* root = NULL;

    go1.Signature = SIGNATURE;
    go2.Signature = SIGNATURE;

    AabbNodeStartup(GetGameObjectBounds);

    go1.Node = AabbNodeInsert(&root, go1.Mins, go1.Maxs, &go1);
    go2.Node = AabbNodeInsert(&root, go2.Mins, go2.Maxs, &go2);

//    AabbNodeRemove(&root, root);
    AabbNodeRemove(&root, go1.Node);
    AabbNodeRemove(&root, go2.Node);

    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(commandLine);
    UNREFERENCED_PARAMETER(commandShow);

    f.X = 3;
    f.F = -234.8f;

    if (!HandleTableStartup())
    {
        return -1;
    }

    h = AllocHandle(&f, 1);
    if (h == InvalidHandle)
    {
        return -2;
    }

    g = (Foo*)GetObjectFromHandle(h);
    if (g == 0)
    {
        return -3;
    }

    FreeHandle(h);

    HandleTableShutdown();

    if (!ConvertCharToWChar(buffer, buffer2, 100))
    {
        return -3;
    }

    if (!ConvertWCharToChar(buffer2, buffer3, 100))
    {
        return -4;
    }

    return 0;
}
