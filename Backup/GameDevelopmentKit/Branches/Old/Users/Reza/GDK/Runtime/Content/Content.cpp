#include <Content.h>
#include "FileBasedContent.h"

namespace GDK
{
    Content::Content()
    {
    }

    std::shared_ptr<Content> Content::CreateFileBased(_In_ const std::wstring& rootFolder)
    {
        return std::shared_ptr<Content>(new (__FILEW__, __LINE__) ContentInternal::FileBasedContent(rootFolder));
    }
}
