#include <Platform.h>
#include <GDKError.h>
#include <Log.h>
#include <MemoryObject.h>

namespace GDK
{
#ifndef USE_GDK_MEMORY_TRACKING

    void* MemoryObject::operator new(_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        return ::operator new (size);
    }

    void* MemoryObject::operator new[](_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        return ::operator new (size);
    }

    void MemoryObject::operator delete(_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        return ::operator delete (p);
    }

    void MemoryObject::operator delete[](_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        return ::operator delete (p);
    }

    void MemoryObject::operator delete(_In_ void* p)
    {
        return ::operator delete (p);
    }

    void MemoryObject::operator delete[](_In_ void* p)
    {
        return ::operator delete (p);
    }
#else

    struct TaggedMemoryBlockHeader
    {
        wchar_t* tag;
        wchar_t* filename;
        uint32_t lineNumber;
        TaggedMemoryBlockHeader* prev;
        TaggedMemoryBlockHeader* next;
    };

    static TaggedMemoryBlockHeader* g_head = nullptr;

    static void TrackNode(_Inout_ TaggedMemoryBlockHeader* block)
    {
        if (g_head == nullptr)
        {
            g_head = block;
        }
        else
        {
            block->next = g_head;
            g_head->prev = block;
            g_head = block;
        }
    }

    static void UntrackNode(_Inout_ TaggedMemoryBlockHeader* block)
    {
        if (block->prev != nullptr)
        {
            block->prev->next = block->next;
        }
        if (block->next != nullptr)
        {
            block->next->prev = block->prev;
        }

        if (g_head == block)
        {
            g_head = block->next;
        }
    }

    static void* AllocateTaggedMemory(_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        size_t blockSize = size + sizeof(TaggedMemoryBlockHeader);

        TaggedMemoryBlockHeader* taggedBlock = reinterpret_cast<TaggedMemoryBlockHeader*>(malloc(blockSize));
        CHECK_NOT_NULL(taggedBlock);

        memset(taggedBlock, 0, blockSize);

        if (tag != nullptr)
        {
            size_t tagSizeWithNull = wcslen(tag) + 1;
            taggedBlock->tag = reinterpret_cast<wchar_t*>(malloc(tagSizeWithNull * sizeof(wchar_t)));
            CHECK_NOT_NULL(taggedBlock->tag);
            wcscpy_s(taggedBlock->tag, tagSizeWithNull, tag);
        }

        size_t filenameSizeWithNull = wcslen(filename) + 1;
        taggedBlock->filename = reinterpret_cast<wchar_t*>(malloc(filenameSizeWithNull * sizeof(wchar_t)));
        CHECK_NOT_NULL(taggedBlock->filename);
        wcscpy_s(taggedBlock->filename, filenameSizeWithNull, filename);

        taggedBlock->lineNumber = lineNumber;

        TrackNode(taggedBlock);

        return reinterpret_cast<byte_t*>(taggedBlock) + sizeof(TaggedMemoryBlockHeader);
    }

    static void FreeTaggedMemory(_In_ void* p)
    {
        if (p != nullptr)
        {
            TaggedMemoryBlockHeader* taggedBlock = reinterpret_cast<TaggedMemoryBlockHeader*>(reinterpret_cast<byte_t*>(p) - sizeof(TaggedMemoryBlockHeader));
            if (taggedBlock)
            {
                UntrackNode(taggedBlock);
                if (taggedBlock->tag)
                {
                    free(taggedBlock->tag);
                }
                if (taggedBlock->filename)
                {
                    free(taggedBlock->filename);
                }
                free(taggedBlock);
            }
        }
    }

    static void ReportMemoryLeaks()
    {
        if (g_head != nullptr)
        {
            static wchar_t message[1000] = {0};

            Log::WriteInfo(L"\n*** GDK Memory Tracking ***\n");
            Log::WriteInfo(L"    LEAKS DETECTED           \n\n");

            TaggedMemoryBlockHeader* node = g_head;
            while (node != nullptr)
            {
                swprintf_s(message, L"Source: %s\nFile: %s\nLine: %d\n\n", node->tag ? node->tag : L"unknown", node->filename, node->lineNumber);
                Log::WriteInfo(message);
                node = node->next;
            }

            Log::WriteInfo(L"\n***************************\n");
        }
    }

    static void EnsureTrackingEnabled()
    {
        static bool enabled = false;
        if (!enabled)
        {
            atexit([]()
            {
                ReportMemoryLeaks();
            });
            enabled = true;
        }
    }

    void* MemoryObject::operator new(_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        EnsureTrackingEnabled();
        return AllocateTaggedMemory(size, tag, filename, lineNumber);
    }

    void* MemoryObject::operator new[](_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        EnsureTrackingEnabled();
        return AllocateTaggedMemory(size, tag, filename, lineNumber);
    }

    void MemoryObject::operator delete(_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        FreeTaggedMemory(p);
    }

    void MemoryObject::operator delete[](_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        FreeTaggedMemory(p);
    }

    void MemoryObject::operator delete(_In_ void* p)
    {
        FreeTaggedMemory(p);
    }

    void MemoryObject::operator delete[](_In_ void* p)
    {
        FreeTaggedMemory(p);
    }
#endif
}
