#pragma once

class CCopiedBuffer
{
public:
    CCopiedBuffer(void* pSrc, size_t _Size)
    {
        // allocate proper size
        BYTE* pData = new BYTE[_Size];
        ptr = pData;
        numBytes = _Size;

        // copy memory
        memcpy(ptr, pSrc, _Size);
    }

    virtual ~CCopiedBuffer()
    {
        if (ptr)
        {
            BYTE* pData = (BYTE*)ptr;
            delete [] pData;
        }
    }

    void Detach()
    {
        ptr = NULL;
        numBytes = 0;
    }

    void* ptr;
    size_t numBytes;
};

typedef std::shared_ptr<CCopiedBuffer> CopiedBufferPtr;
