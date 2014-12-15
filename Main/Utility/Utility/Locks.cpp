#include <Utility.h>
#include <windows.h>
#include <intrin.h>
#include <assert.h>

_Use_decl_annotations_
void UMSpinLockInitialize(UMSpinLock* lock)
{
    lock->value = 0;
}

_Use_decl_annotations_
bool UMSpinLockTryAcquire(UMSpinLock* lock, uint32_t spinCount)
{
    uint32_t count = 0;
    while (count++ < spinCount)
    {
        if (lock->value == 0)
        {
            if (_InterlockedExchange(&lock->value, 1) == 0)
            {
                return true;
            }
        }

        YieldProcessor();
    }

    return false;
}

_Use_decl_annotations_
void UMSpinLockAcquire(UMSpinLock* lock)
{
    for (;;)
    {
        if (lock->value == 0)
        {
            if (_InterlockedExchange(&lock->value, 1) == 0)
            {
                return;
            }
        }

        YieldProcessor();
    }
}

_Use_decl_annotations_
void UMSpinLockRelease(UMSpinLock* lock)
{
    _InterlockedExchange(&lock->value, 0);
}

_Use_decl_annotations_
void UMCriticalSectionInitialize(UMCriticalSection* cs, uint16_t maxWaitChain)
{
    UMSpinLockInitialize(&cs->Lock);
    cs->CurrentOwner = (uint32_t)-1;
    cs->Head = 0;
    cs->Tail = 0;
    cs->ThreadWaitQueue = (uint32_t*)malloc(maxWaitChain * sizeof(uint32_t));
    cs->WaitQueueLength = maxWaitChain;
}

_Use_decl_annotations_
void UMCriticalSectionDestroy(UMCriticalSection* cs)
{
    free(cs->ThreadWaitQueue);
}

_Use_decl_annotations_
void UMCriticalSectionAcquire(UMCriticalSection* cs)
{
    uint32_t currentThread = (uint32_t)GetCurrentThreadId();
    if (cs->CurrentOwner != currentThread)
    {
        UMSpinLockAcquire(&cs->Lock);

        if (cs->CurrentOwner != currentThread)
        {
            if (cs->CurrentOwner == (uint32_t)-1)
            {
                cs->CurrentOwner = currentThread;
            }
            else
            {
                cs->ThreadWaitQueue[cs->Tail] = currentThread;
                cs->Tail = (cs->Tail + 1) % cs->WaitQueueLength;
                uint32_t resume = cs->CurrentOwner;

                UMSpinLockRelease(&cs->Lock);

                // We need to wait
                HANDLE resumeThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, resume);
                if (resumeThread != nullptr)
                {
                    ResumeThread(resumeThread);
                    CloseHandle(resumeThread);
                }

                while (cs->CurrentOwner != currentThread)
                {
                    SuspendThread(GetCurrentThread());
                    YieldProcessor();
                }
                return;
            }
        }

        UMSpinLockRelease(&cs->Lock);
    }
}

_Use_decl_annotations_
void UMCriticalSectionRelease(UMCriticalSection* cs)
{
#if defined(_DEBUG)
    uint32_t currentThread = (uint32_t)GetCurrentThreadId();
    assert(currentThread == cs->CurrentOwner);
#endif

    UMSpinLockAcquire(&cs->Lock);

    if (cs->Head != cs->Tail)
    {
        cs->CurrentOwner = cs->ThreadWaitQueue[cs->Head];
        cs->Head = (cs->Head + 1) % cs->WaitQueueLength;
        uint32_t resume = cs->CurrentOwner;

        UMSpinLockRelease(&cs->Lock);

        HANDLE resumeThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, resume);
        if (resumeThread != nullptr)
        {
            ResumeThread(resumeThread);
            CloseHandle(resumeThread);
        }
        return;
    }
    else
    {
        cs->CurrentOwner = (uint32_t)-1;
    }

    UMSpinLockRelease(&cs->Lock);
}
