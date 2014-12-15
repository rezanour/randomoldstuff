#include <AudioContent.h>
#include <GDKError.h>

using namespace GDK;

struct WavHeader
{
    char rID[4]; // "RIFF"
    int32_t length;
    char wID[4]; // "WAVE"
    char fID[4]; // "fmt "
    int32_t pcmHeaderLength;
    int16_t formatTag;
    int16_t channels;
    int32_t samplesPerSecond;
    int32_t averageBytesPerSecond;
    int16_t blockAlign;
    int16_t bitsPerSample;
    char cID[4]; // "data"
    int32_t dataSize;
};

_Use_decl_annotations_
std::shared_ptr<AudioContent> AudioContent::Create(std::istream& stream)
{
    WavHeader header = {0};
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    CHECK_TRUE(_stricmp(header.rID, "RIFF"));
    CHECK_TRUE(_stricmp(header.wID, "WAVE"));
    CHECK_TRUE(_stricmp(header.fID, "fmt "));
    CHECK_TRUE(_stricmp(header.cID, "data"));    
    std::unique_ptr<byte_t[]> chunkData(new byte_t[header.dataSize]);
    stream.read(reinterpret_cast<char *>(chunkData.get()), header.dataSize);
    return std::shared_ptr<AudioContent>(GDKNEW AudioContent(header.channels, header.samplesPerSecond, header.bitsPerSample, header.dataSize, chunkData));
}

_Use_decl_annotations_
AudioContent::AudioContent(int16_t channels, int32_t samplesPerSecond, int16_t bitsPerSample, int32_t dataLength, std::unique_ptr<byte_t[]>& data) :
    _channels(channels), _samplesPerSecond(samplesPerSecond), _bitsPerSample(bitsPerSample), _dataLength(dataLength)
{
    _data.swap(data);
}