#include "DirectoryContentVolume.h"
#include <GDKError.h>

using namespace GDK;

_Use_decl_annotations_
DirectoryContentVolume::DirectoryContentVolume(const std::wstring &root, const std::wstring &logicalRoot, int32_t priority)
    : ContentVolume(root, logicalRoot, priority)
{
}

_Use_decl_annotations_
std::shared_ptr<std::istream> DirectoryContentVolume::OpenFile(const std::wstring &filename)
{
    std::wstring fullPath = filename;

    // If this volume has a logical root associated with it 
    if (_logicalRoot.size() > 0)
    {
        // If the filename is shorter than the logical root, it can't be in this volume
        if (filename.size() < _logicalRoot.size())
        {
            return nullptr;
        }

        // Ensure the filename starts with the logical root
        if (!std::equal(std::begin(_logicalRoot), std::end(_logicalRoot), std::begin(filename)))
        {
            return nullptr;
        }

        // Remove the logical root from the filename
        fullPath = fullPath.substr(_logicalRoot.size());
    }

    // Prepend the real root onto the path to get a full file path
    fullPath = _realRoot + L"\\" + fullPath;

    // The ifstream defined in the C++ spec doesn't support wide strings so we must convert our path to ANSI
    std::string finalPath(std::begin(fullPath), std::end(fullPath));

    // Open a file stream to the full path of the file
    std::unique_ptr<std::ifstream> fileStream(new std::ifstream(finalPath.c_str(), std::ios_base::in | std::ios_base::binary));

    // If the file stream is good, the file exists and we'll release it from the unique pointer and return it in a shared_ptr<istream>
    if (fileStream->good())
    {
        return std::shared_ptr<std::istream>(fileStream.release());
    }
    
    return nullptr;
}