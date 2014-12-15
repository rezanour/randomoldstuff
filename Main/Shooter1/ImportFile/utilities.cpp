#include "precomp.h"

void ReadFileToBuffer(_In_ const char* path, _Inout_ std::vector<uint8_t>& buffer)
{
    buffer.clear();
    std::ifstream inputfile(path, std::ios::in | std::ios_base::binary | std::ios::ate);
    if (inputfile.is_open())
    {
        buffer.resize((uint32_t)inputfile.tellg());
        inputfile.seekg(0, std::ios::beg);
        inputfile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        inputfile.close();
    }
}