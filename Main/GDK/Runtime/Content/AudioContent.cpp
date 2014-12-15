#include <AudioContent.h>
#include <GDKError.h>

using namespace GDK;

struct WavHeader
{
    char id[4]; // "RIFF"
    int32_t length;
    char waveId[4]; // "WAVE"
};

struct FormatChunkHeader
{
    char id[4]; // "fmt "
    int32_t chunkSize;
};

struct FormatChunk
{
    int16_t formatTag;
    int16_t channels;
    int32_t samplesPerSecond;
    int32_t averageBytesPerSecond;
    int16_t blockAlign;
    int16_t bitsPerSample;
	int16_t size;
	int16_t validBitsPerSample;
	int32_t channelMask;
	byte_t subFormat[16];
};

struct DataChunkHeader
{
    char id[4]; // "data"
    int32_t dataSize;
};

_Use_decl_annotations_
std::shared_ptr<AudioContent> AudioContent::Create(std::istream& stream)
{
    WavHeader header = {0};
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    CHECK_TRUE(_stricmp(header.id, "RIFF"));
    CHECK_TRUE(_stricmp(header.waveId, "WAVE"));

	FormatChunkHeader fmtHeader = {0};
	stream.read(reinterpret_cast<char*>(&fmtHeader), sizeof(fmtHeader));
    CHECK_TRUE(_stricmp(fmtHeader.id, "fmt "));

	FormatChunk fmtChunk = {0};
	stream.read(reinterpret_cast<char*>(&fmtChunk), fmtHeader.chunkSize);

	DataChunkHeader dataHeader = {0};
	stream.read(reinterpret_cast<char*>(&dataHeader), sizeof(dataHeader));
    CHECK_TRUE(_stricmp(dataHeader.id, "data"));    

    std::unique_ptr<byte_t[]> chunkData(new byte_t[dataHeader.dataSize]);
    stream.read(reinterpret_cast<char *>(chunkData.get()), dataHeader.dataSize);

    return std::shared_ptr<AudioContent>(GDKNEW AudioContent(
		fmtChunk.channels,
		fmtChunk.samplesPerSecond, 
		fmtChunk.bitsPerSample, 
		dataHeader.dataSize, 
		chunkData));
}

_Use_decl_annotations_
AudioContent::AudioContent(int16_t channels, int32_t samplesPerSecond, int16_t bitsPerSample, int32_t dataLength, std::unique_ptr<byte_t[]>& data) :
    _channels(channels), _samplesPerSecond(samplesPerSecond), _bitsPerSample(bitsPerSample), _dataLength(dataLength)
{
    _data.swap(data);
}