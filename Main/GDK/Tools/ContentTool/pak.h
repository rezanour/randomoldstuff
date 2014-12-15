#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#pragma pack(push,1)

struct pak_fileheader_t
{
    char signature[4]; 
    int dirOffset;
    int dirLength;
};

struct pak_entry_t
{
    char filename[56];
    int offset;
    int length;
};

#pragma pack(pop)

void ReadPakEntryList(_In_ std::wstring path, _Inout_ std::vector<pak_entry_t>& entries);
void ReadPakEntryToBuffer(_In_ std::ifstream& inputfile, _In_ const pak_entry_t& entry, _Inout_ std::vector<uint8_t>& buffer);
void ReadPakEntryToBuffer(_In_ std::ifstream& inputfile, _In_ std::vector<pak_entry_t>& entries, _In_ const std::string& entry, _Inout_ std::vector<uint8_t>& buffer);
std::string GetPakEntryName(_In_ const pak_entry_t& entry);