#pragma once

#include <Platform.h>
#include <Content.h>
#include <string>
#include <fstream>

namespace GDK
{
    // Abstract base type for all content volumes
    class ContentVolume : NonCopyable
    {
    public:
        virtual std::shared_ptr<std::istream> OpenFile(_In_ const std::wstring &filename) = 0;

        std::wstring GetRoot() const { return _realRoot; }
        std::wstring GetLogicalRoot() const { return _logicalRoot; }
        int32_t GetPriority() const { return _priority; }

    protected:
        ContentVolume(_In_ const std::wstring &root, _In_ const std::wstring &logicalRoot, _In_ int32_t priority) : _realRoot(root), _logicalRoot(logicalRoot), _priority(priority)
        {
        }
        
        const std::wstring _realRoot;
        const std::wstring _logicalRoot;
        const int32_t _priority;
    };
}
