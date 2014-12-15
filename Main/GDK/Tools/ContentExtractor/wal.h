#pragma once

#pragma pack(push,1)

struct WAL_HEADER
{
    char         name[32];
    unsigned int width;         // width (in pixels) of the largest mipmap level
    unsigned int height;        // height (in pixels) of the largest mipmap level
    int          offset[4];     // byte offset of the start of each of the 4 mipmap levels
    char         next_name[32]; // name of the next texture in the animation
    unsigned int flags;
    unsigned int contents;
    unsigned int value;
};

#pragma pack(pop)

HRESULT SaveWALAsTextureContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size, std::vector<RGBQUAD> bmpal);
