#include "precomp.h"

_Use_decl_annotations_
void LoadImageFileMetadataOnly(const char* path, ImageFileData* fileData)
{
    assert(strlen(path) < MAX_PATH);

    wchar_t source[MAX_PATH];
    wsprintf(source, L"%S", path);

    TexMetadata metadata;

    if (FAILED(GetMetadataFromDDSFile(source, DDS_FLAGS_NONE, metadata)))
    {
        if (FAILED(GetMetadataFromTGAFile(source, metadata)))
        {
            if (FAILED(GetMetadataFromWICFile(source, WIC_FLAGS_NONE, metadata)))
            {
                Error("Unsupported image file format");
            }
        }
    }

    ZeroMemory(fileData, sizeof(ImageFileData));
    fileData->Width = (uint32_t)metadata.width;
    fileData->Height = (uint32_t)metadata.height;
    fileData->Format = metadata.format;
    fileData->MipCount = (uint32_t)metadata.mipLevels;

    size_t rowPitch, slicePitch;
    ComputePitch(fileData->Format, fileData->Width, fileData->Height, rowPitch, slicePitch);

    fileData->Pitch = static_cast<uint32_t>(rowPitch);
}

void LoadImageFileData(_In_z_ const char* path, _Out_ ImageFileData* fileData)
{
    std::string source(path);
    std::wstring wide_filename(source.size(), L' ');
    std::copy(std::begin(source), std::end(source), std::begin(wide_filename));

    TexMetadata metadata;
    ScratchImage image;
    ScratchImage mipMaps;
    if (FAILED(LoadFromDDSFile(wide_filename.c_str(), DDS_FLAGS_NONE, &metadata, image)))
    {
        if (FAILED(LoadFromTGAFile(wide_filename.c_str(), &metadata, image)))
        {
            if (FAILED(LoadFromWICFile(wide_filename.c_str(), WIC_FLAGS_NONE, &metadata, image)))
            {
                Error("Unsupported image file format");
            }
        }
    }

    if (FAILED(GenerateMipMaps(*image.GetImage(0,0,0),TEX_FILTER_CUBIC,0,mipMaps,false)))
    {
        Error("Mipmap creation failed");
    }

    fileData->Width = static_cast<uint32_t>(metadata.width);
    fileData->Height = static_cast<uint32_t>(metadata.height);
    fileData->Data = mipMaps.GetPixels();
    fileData->DataSize = static_cast<uint32_t>(mipMaps.GetPixelsSize());
    fileData->Format = metadata.format;
    fileData->MipCount = static_cast<uint8_t>(mipMaps.GetImageCount());

    // Compute pitch
    size_t rowPitch = 0;
    size_t slicePitch = 0;
    ComputePitch(fileData->Format, fileData->Width, fileData->Height, rowPitch, slicePitch);

    fileData->Pitch = static_cast<uint32_t>(rowPitch);

    mipMaps.DetachPixels(); // Give ownership of pixel data to caller
}

void FreeImageFileData(_In_ ImageFileData* fileData)
{
    if (fileData)
    {
        if (fileData->Data)
        {
            _aligned_free(fileData->Data);
        }

        memset(fileData, 0, sizeof(*fileData));
    }
}

