#pragma once

#include <vector>

namespace GDK {
namespace Content {

    #pragma pack(push,1)

    const uint32_t CONTENT_ARCHIVE_VERSION = 0x20314143; // "CA1 "
    const uint32_t FILEBASED_CONTENT_ARCHIVE_VERSION = 0x31414346; // "FCA1"

    struct CONTENT_ARCHIVE_FILEHEADER
    {
        uint32_t         Version;   // set to CONTENT_ARCHIVE_VERSION
        uint64_t         FileCount; // number of files
        // [CONTENT_ARCHIVE_ENTRY]
        // [CONTENT_ARCHIVE_ENTRY]
        // .
        // .
        // .
    };

    struct CONTENT_ARCHIVE_ENTRY
    {
        char        Name[256];        
        uint32_t    Size;
        uint32_t    Offset;
    };

    // File-based content archives are marked with a file version of FILEBASED_CONTENT_ARCHIVE_VERSION.
    // The next data is a path to the root of the directory where the content is located.
    //
    // Example archive:
    // FCA1c:\gamedata

    struct COMMON_RESOURCE_FILEHEADER
    {
        uint32_t         Version;  // set to XXXX_RESOURCE_VERSION
        char             Name[64]; // name of resource
        uint64_t         Id;
    };

    const uint32_t GEOMETRY_RESOURCE_VERSION = 0x20315247; // "GR1 "
    
    struct GEOMETRY_RESOURCE_FILEHEADER
    {
        COMMON_RESOURCE_FILEHEADER Header;
        uint32_t Format;
        uint32_t FrameCount;
    };

    struct GEOMETRY_RESOURCE_FRAME
    {
        char name[64];
        uint32_t VertexCount;
        uint32_t IndicesCount;
    };

    // The Geometry Resource file is constructed as follows:

    // [GEOMETRY_RESOURCE_FILEHEADER ]  contains version, metadata
    // [GEOMETRY_RESOURCE_FRAME      ]  contains individual frame
    // [GEOMETRY_RESOURCE_FRAME      ]            .
    // [GEOMETRY_RESOURCE_FRAME      ]            .

    const uint32_t TEXTURE_RESOURCE_VERSION = 0x20315254; // "TR1 "
    
    struct TEXTURE_RESOURCE_IMAGE
    {
        size_t      width;
        size_t      height;
        TEXTURE_RESOURCE_FORMAT format;
        size_t      rowPitch;
        size_t      slicePitch;
        uint32_t    cbPixels;
        std::vector<uint8_t> pixels;
    };

    enum TEXTURE_RESOURCE_DIMENSION
    {
        TEXTURE_RESOURCE_DIMENSION_UNKNOWN      = 0,
        TEXTURE_RESOURCE_DIMENSION_TEXTURE1D    = 2,
        TEXTURE_RESOURCE_DIMENSION_TEXTURE2D    = 3,
        TEXTURE_RESOURCE_DIMENSION_TEXTURE3D    = 4,
    };

    enum TEXTURE_RESOURCE_MISC_FLAG
    {
       TEXTURE_RESOURCE_MISC_TEXTURECUBE = 0x4L,
    };

    struct TEXTURE_RESOURCE_METADATA
    {
        size_t          width;
        size_t          height;     // Should be 1 for 1D textures
        size_t          depth;      // Should be 1 for 1D or 2D textures
        size_t          arraySize;  // For cubemap, this is a multiple of 6
        size_t          mipLevels;
        uint32_t        miscFlags;
        TEXTURE_RESOURCE_FORMAT     format;
        TEXTURE_RESOURCE_DIMENSION dimension;
    };

    struct TEXTURE_RESOURCE_FILEHEADER
    {
        COMMON_RESOURCE_FILEHEADER header;
        TEXTURE_RESOURCE_METADATA metadata;
    };

    // The Texture Resource file is constructed as follows:

    // [TEXTURE_RESOURCE_FILEHEADER ]  contains version, metadata
    // [TEXTURE_RESOURCE_IMAGE      ]  contains individual image
    // [TEXTURE_RESOURCE_IMAGE      ]            .
    // [TEXTURE_RESOURCE_IMAGE      ]            .
    // [TEXTURE_RESOURCE_IMAGE      ]            .

    const uint32_t WORLD_RESOURCE_VERSION = 0x20315257; // "WR1 "
    
    const uint32_t MAX_WORLD_NAME_LENGTH = 128;

    struct WORLD_RESOURCE_FILEHEADER
    {
        COMMON_RESOURCE_FILEHEADER header;
        wchar_t name[MAX_WORLD_NAME_LENGTH];
        uint32_t numObjects;
    };

    // The World Resource file is constructed as follows:

    // [WORLD_RESOURCE_FILEHEADER ]  contains version, metadata
    // [WORLD_RESOURCE_OBJECT     ]  contains individual object
    // [WORLD_RESOURCE_OBJECT     ]            .
    // [WORLD_RESOURCE_OBJECT     ]            .
    // [WORLD_RESOURCE_OBJECT     ]            .

    #pragma pack(pop)

} // Content
} // GDK
