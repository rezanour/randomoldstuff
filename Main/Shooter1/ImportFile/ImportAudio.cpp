#include "precomp.h"

void LoadSoundFileData(_In_z_ const char* path, _Out_ SoundFileData* fileData)
{
    WAVData wavData;

    std::ifstream inputfile(path, std::ios::in | std::ios_base::binary | std::ios::ate);
    if (inputfile.is_open())
    {
        fileData->DataSize = (uint32_t)inputfile.tellg();
        fileData->Data = new uint8_t[fileData->DataSize];//reinterpret_cast<uint8_t*>(_aligned_malloc(fileData->DataSize, 16));
        inputfile.seekg(0, std::ios::beg);
        inputfile.read(reinterpret_cast<char*>(fileData->Data), fileData->DataSize);
        inputfile.close();
    }

    if (FAILED(LoadWAVAudioInMemoryEx(fileData->Data, fileData->DataSize, wavData)))
    {
        throw std::exception("Unsupported sound file format");
    }

    fileData->Format = wavData.wfx;
    fileData->StartAudioData = wavData.startAudio;
    fileData->StartAudioDataSize = wavData.audioBytes;
}

void FreeSoundFileData(_In_ SoundFileData* fileData)
{
    if (fileData)
    {
        if (fileData->Data)
        {
            delete[] fileData->Data; //_aligned_free(fileData->Data);
        }

        memset(fileData, 0, sizeof(*fileData));
    }
}
