#include "pak.h"
#include "helpers.h"

void ReadPakEntryList(_In_ std::wstring path, _Inout_ std::vector<pak_entry_t>& entries)
{
    std::ifstream inputfile(path,std::ios::in|std::ios_base::binary);
    if (inputfile.is_open())
    {
        entries.clear();
        pak_fileheader_t pakHeader;
        uint32_t numEntries = 0;
        inputfile.read(reinterpret_cast<char*>(&pakHeader), sizeof(pakHeader));
        numEntries = pakHeader.dirLength/sizeof(pak_entry_t);
        inputfile.seekg(pakHeader.dirOffset);
        for (size_t i = 0; i < numEntries; i++)
        {
            pak_entry_t entry;
            inputfile.read(reinterpret_cast<char*>(&entry), sizeof(entry));
            entries.push_back(entry);
        }
    }
    else
    {
        printf("Cannot read PAK entry list from %ws\n", path.c_str());
    }
}

void ReadPakEntryToBuffer(_In_ std::ifstream& inputfile, _In_ const pak_entry_t& entry, _Inout_ std::vector<uint8_t>& buffer)
{
    inputfile.seekg (0, std::ios::beg);
    inputfile.seekg(entry.offset);
    buffer.resize(entry.length);
    inputfile.read(reinterpret_cast<char*>(buffer.data()), entry.length);
}

void ReadPakEntryToBuffer(_In_ std::ifstream& inputfile, _In_ std::vector<pak_entry_t>& entries, _In_ const std::string& entry, _Inout_ std::vector<uint8_t>& buffer)
{
    for(size_t i = 0; i < entries.size(); i++)
    {
        if (_strcmpi(entry.c_str(), GetPakEntryName(entries[i]).c_str()) == 0)
        {
            ReadPakEntryToBuffer(inputfile, entries[i], buffer);
            break;
        }
    }
}

std::string GetPakEntryName(_In_ const pak_entry_t& entry)
{
    std::string entryName;
    for (uint8_t i = 0; i < _countof(entry.filename); i++)
    {
        if (entry.filename[i] == 0)
        {
            break;
        }
        entryName += entry.filename[i];
    }

    return entryName;
}
