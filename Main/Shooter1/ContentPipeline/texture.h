#pragma once

class TextureContent
{
public:
    static std::unique_ptr<TextureContent> Load(_In_z_ const wchar_t* filename);

    // Save to the current location in the given file
    void Save(Output& output) const;

    uint32_t GetArraySize() const;
    uint32_t GetMipCount() const;

    void GenerateMips();

    //
    // NOTE: Resizing an image destroys the mip chain, if one exists.
    //
    void ResizeImage(uint32_t width, uint32_t height);

private:
    TextureContent(ScratchImage&& image);

    // Prevent copy
    TextureContent(const TextureContent&);
    TextureContent& operator= (const TextureContent&);

private:
    ScratchImage    _image;
};
