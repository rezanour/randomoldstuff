#include "stdafx.h"

#define CHECKHR(stmt, msg) hr = (stmt); if (FAILED(hr)) { wprintf(L"%ws, error is %08.8lx", msg, hr); goto Exit;}

using namespace Microsoft::WRL;

HRESULT SaveGeometryResourceAsWaveFrontOBJ(const wchar_t* path, GEOMETRY_RESOURCE_VERTEX* vertices, size_t numVertices, uint32_t* indices, size_t numIndices );
HRESULT SaveMD2AsGeometryResource(const wchar_t* path, const char* name, byte_t* data, uint32_t size);
HRESULT SavePCXAsTextureResource(const wchar_t* path, const char* name, byte_t* data, uint32_t size);
HRESULT ConvertPCXToBMP(PCXFILEHEADER* header, byte_t* data, uint32_t size, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmheader, bool flipImage = false);
HRESULT SaveBMP(const wchar_t* path, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmheader);
HRESULT GenerateNormals(GEOMETRY_RESOURCE_VERTEX* vertices, size_t numVertices, uint32_t* indices, size_t numIndices);
HRESULT Convert8bitBMPto24bit(std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih);

HRESULT SeekTo(_In_ IStream* stream, _In_ DWORD dwOrigin, _In_ LONG offset)
{            
    LARGE_INTEGER liMove = {0};
    ULARGE_INTEGER uliPos = {0};
    liMove.LowPart = offset;

    return stream->Seek(liMove, dwOrigin, &uliPos);
}

DWORD GetStreamSize(_In_ IStream* stream)
{
    STATSTG statstg;
    DWORD streamSize = 0;
    stream->Stat(&statstg, STATFLAG_DEFAULT);

    return statstg.cbSize.LowPart;
}

HRESULT GetPakChunk(_In_ const wchar_t* pakFilePath, _In_ const char* name, _Out_ std::vector<byte_t>& data)
{
    if (!pakFilePath ||!name)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;

    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        // If an empty name is passed, make lots of noise by echoing out all entries
        if (strlen(name) == 0)
        {
            // output PAK entries
            printf("entry:  %s\n"
                   "length: %d\n"
                   "offset: %d\n", entry.filename, entry.length, entry.offset);
            printf("===========================================\n");
        }

        if (strcmpi(name, entry.filename) == 0)
        {
            data.resize(entry.length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entry.offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entry.length, &bytesRead), L"Error reading PAK file entry data");
            break;
        }
    }

Exit:

    return hr;
}

struct QUAKE_CONTENT
{
    char* name;
    char* md2Model;
    char* skin1;
    char* skin2;
    wchar_t* gameContentPath;
};

QUAKE_CONTENT quake_content[] = 
{
    {"soldier","models/monsters/soldier/tris.md2", "models/monsters/soldier/skin.pcx", "models/monsters/soldier/pain.pcx", L"..\\..\\..\\Quake2\\Content\\enemies\\soldier.object\\"},
    {"medkit_medium","models/items/healing/medium/tris.md2", "models/items/healing/medium/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_medium.object\\"},
    {"medkit_large","models/items/healing/large/tris.md2", "models/items/healing/large/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_large.object\\"},
    {"medkit_stimpack","models/items/healing/stimpack/tris.md2", "models/items/healing/stimpack/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_stimpack.object\\"},
    {"bfg","models/weapons/g_bfg/tris.md2", "models/weapons/g_bfg/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\weapons\\bfg.object\\"},
    {"brain","models/monsters/brain/tris.md2", "models/monsters/brain/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\enemies\\brain.object\\"},
    
};

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    std::vector<byte_t> data;

    // uncomment to list ALL PAK file chunks
    //CHECKHR(GetPakChunk(argv[1], "", data), L"Error listing all chunks");

    // Extract md2 objects
    for (size_t i = 0; i < ARRAYSIZE(quake_content); i++)
    {
        std::wstring contentDir;

        contentDir = quake_content[i].gameContentPath;
        contentDir.append(L"mesh.geometry");
        CHECKHR(GetPakChunk(argv[1], quake_content[i].md2Model, data), L"Error finding md2 model");
        SaveMD2AsGeometryResource(contentDir.c_str(), quake_content[i].name, data.data(), data.size());

        data.clear();

        contentDir = quake_content[i].gameContentPath;
        contentDir.append(L"primary.texture");
        CHECKHR(GetPakChunk(argv[1], quake_content[i].skin1, data), L"Error finding md2 primary texture");
        SavePCXAsTextureResource(contentDir.c_str(), quake_content[i].name, data.data(), data.size());
    }

Exit:

    return 0;
}

HRESULT SavePCXAsTextureResource(const wchar_t* path, const char* name, byte_t* data, uint32_t size)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    
    ComPtr<IStream> resourceFile;
    TEXTURE_RESOURCE_FILEHEADER trheader = {0};
    TEXTURE_RESOURCE_IMAGE trimage = {0};

    ComPtr<IStream> bmpFile;
    std::vector<byte_t> bmpData;
    std::vector<RGBQUAD> bmpal;
    BITMAPINFOHEADER bmih = {0};

    ULONG bytesWritten = 0;
    PCXFILEHEADER* header = (PCXFILEHEADER*)data;
    printf("PCX details\n"
            "  name:     %s\n"
            "  id:       %d\n"
            "  version:  %d\n"
            "  encoding: %d\n"
            "  bpp:      %d\n"
            "  x1:       %d\n"
            "  y1:       %d\n"
            "  x2:       %d\n"
            "  y2:       %d\n"
            "  hres:     %d\n"
            "  yres:     %d\n"
            "  planes:   %d\n"
            "  bpl:      %d\n"
            "  pal:      %d\n", name, header->id, header->version, header->encoding, header->bpp, 
                                header->x1, header->y1, header->x2, header->y2, header->hres,
                                header->yres, header->planes, header->bpl, header->pal);

    CHECKHR(ConvertPCXToBMP(header, data, size, bmpData, bmpal, bmih), L"Failed to conver PCX to BMP");
    CHECKHR(SaveBMP(L"x64\\debug\\debug_texture.bmp", bmpData, bmpal, bmih), L"Failed to save BMP file");

    CHECKHR(Convert8bitBMPto24bit(bmpData, bmpal, bmih), L"Failed to convert 8-bit data to 24-bit BMP");
    CHECKHR(SaveBMP(L"x64\\debug\\debug_texture32.bmp", bmpData, bmpal, bmih), L"Failed to save 24 bit BMP file");

    trheader.header.Version = TEXTURE_RESOURCE_VERSION;
    trheader.metadata.arraySize = 1;
    trheader.metadata.depth     = 1;
    trheader.metadata.height    = bmih.biHeight;
    trheader.metadata.width     = bmih.biWidth;
    trheader.metadata.format    = TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM;
    trheader.metadata.dimension = TEXTURE_RESOURCE_DIMENSION_TEXTURE2D;

    trimage.cbPixels   = bmpData.size();
    trimage.format     = trheader.metadata.format;
    trimage.height     = trheader.metadata.height;
    trimage.width      = trheader.metadata.width;
    trimage.rowPitch   = trimage.width * 4;
    trimage.slicePitch = trimage.cbPixels;

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &resourceFile), L"Error creating texture resource file");
    CHECKHR(resourceFile->Write(&trheader, sizeof(trheader), &bytesWritten), L"Failed to write texture resource file header");
    CHECKHR(resourceFile->Write(&trimage, sizeof(trimage), &bytesWritten), L"Failed to write texture resource image metadata header");
    CHECKHR(resourceFile->Write(bmpData.data(), bmpData.size(), &bytesWritten), L"Failed to write texture resource image data");

Exit:

    return hr;
}

HRESULT Convert8bitBMPto24bit(std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih)
{
    std::vector<byte_t> bmpData32;
    for (size_t i = 0; i < bmpData.size();i++)
    {
        bmpData32.push_back(bmpal[bmpData[i]].rgbRed);   // R
        bmpData32.push_back(bmpal[bmpData[i]].rgbGreen); // G
        bmpData32.push_back(bmpal[bmpData[i]].rgbBlue);  // B
        bmpData32.push_back(255);                        // A
    }

    bmih.biBitCount = 32;
    bmih.biSizeImage *= 4;

    bmpData.resize(bmpData32.size());
    memcpy(bmpData.data(), bmpData32.data(), bmpData32.size());

    return S_OK;
}

HRESULT SaveBMP(const wchar_t* path, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> bmpFile;
    BITMAPFILEHEADER bmfh = {0};
    ULONG bytesWritten = 0;

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &bmpFile), L"Error creating BMP file");

    bmfh.bfSize      = sizeof(BITMAPFILEHEADER);
    bmfh.bfType      = 0x4D42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if (bmih.biBitCount == 8)
    {
        bmfh.bfOffBits += bmpal.size();
    }

    CHECKHR(bmpFile->Write(&bmfh, sizeof(bmfh), &bytesWritten), L"Failed writing BMP file header");
    CHECKHR(bmpFile->Write(&bmih, sizeof(bmih), &bytesWritten), L"Failed writing BMP information header");
    if (bmih.biBitCount == 8)
    {
        CHECKHR(bmpFile->Write(bmpal.data(), bmpal.size() * sizeof(RGBQUAD), &bytesWritten), L"Failed writing BMP palette");
    }
    CHECKHR(bmpFile->Write(bmpData.data(), bmpData.size(), &bytesWritten), L"Failed writing BMP file data");

Exit:

    return hr;
}

HRESULT SaveGeometryResourceAsWaveFrontOBJ(const wchar_t* path, GEOMETRY_RESOURCE_VERTEX* vertices, size_t numVertices, uint32_t* indices, size_t numIndices )
{
    if (!path || !vertices || !numVertices || !indices || !numIndices)
    {
        return E_INVALIDARG;
    }

    const char* objHeader =   "# GDK Geometry Resource Conversion 1.00\r\n";
    const char* objName =     "o %s\r\n";
    const char* objVertex =   "v %f %f %f\r\n";
    const char* objTriangle = "f %d %d %d\r\n";

    HRESULT hr = S_OK;
    ComPtr<IStream> objFile;
    std::string objFileData;
    ULONG bytesWritten = 0;
    char temp[255];

    // Write header
    ZeroMemory(temp, sizeof(temp));
    sprintf(temp, objHeader);
    objFileData.append(temp);

    // Write object name
    ZeroMemory(temp, sizeof(temp));
    sprintf(temp, objName, "GeometryResource");
    objFileData.append(temp);

    for (size_t v = 0; v < numVertices; v++)
    {
        // Write vertex
        ZeroMemory(temp, sizeof(temp));
        sprintf(temp, objVertex, vertices[v].Position.x, vertices[v].Position.y, vertices[v].Position.z);
        objFileData.append(temp);
    }

    for (size_t i = 0; i < numIndices; i+=3)
    {
        // Write Indices
        ZeroMemory(temp, sizeof(temp));
        sprintf(temp, objTriangle, indices[i]+1, indices[i+1]+1, indices[i+2]+1);
        objFileData.append(temp);
    }

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &objFile), L"Error creating wavefront obj file");
    CHECKHR(objFile->Write(objFileData.data(), objFileData.size() * sizeof(char), &bytesWritten), L"Error writing wavefront obj data");

Exit:

    return hr;
}

HRESULT SaveMD2AsGeometryResource(const wchar_t* path, const char* name, byte_t* data, uint32_t size)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ComPtr<IStream> resourceFile;
    ULONG bytesWritten = 0;
    
    MD2_HEADER* header = (MD2_HEADER*)data;
    GEOMETRY_RESOURCE_VERTEX vertex = {0};
    GEOMETRY_RESOURCE_FILEHEADER grheader = {0};
    MD2_VERTEX* vertices = nullptr;
    MD2_TRIANGLE* tris = (MD2_TRIANGLE*)(data + header->ofs_tris);
    MD2_TEXTURE_COORDINATE* textureCoords = (MD2_TEXTURE_COORDINATE*)(data + header->ofs_st);
    MD2_ANIMATION_FRAME* frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames);

    struct md2FrameData
    {
        GEOMETRY_RESOURCE_FRAME frame;
        std::vector<GEOMETRY_RESOURCE_VERTEX> grvertices;
        std::vector<uint32_t> grindices;
    };

    std::vector<md2FrameData> md2Frames;

    printf("MD2 details\n"
            "  name:       %s\n"
            "  skinwidth:  %d\n"
            "  skinheight: %d\n"
            "  framesize:  %d\n"
            "  num_skins:  %d\n"
            "  num_xyz:    %d\n"
            "  num_st:     %d\n"
            "  num_tris:   %d\n"
            "  num_glcmds: %d\n"
            "  num_frames: %d\n", name, header->skinwidth, header->skinheight, header->framesize, header->num_skins, 
                                  header->num_xyz, header->num_st, header->num_tris, header->num_glcmds, header->num_frames);
    
    // Writing frames
    grheader.Format = GEOMETRY_RESOURCE_FORMAT_POSITION | GEOMETRY_RESOURCE_FORMAT_NORMAL | GEOMETRY_RESOURCE_FORMAT_TEXTURE0;

    for (int frameIndex = 0; frameIndex < header->num_frames; frameIndex++)
    {
        frames = (MD2_ANIMATION_FRAME*)(data + header->ofs_frames + (frameIndex * header->framesize));

        vertices = frames->verts;
        int indice = 0;

        md2FrameData frameData;

        ZeroMemory(frameData.frame.name, sizeof(frameData.frame.name));
        memcpy(frameData.frame.name, frames->name, sizeof(frames->name));

        // traverse triangles
        for (int t = 0; t < header->num_tris; t++)
        {
            // traverse vertices
            for (int v = 0; v < 3; v++)
            {
                GEOMETRY_RESOURCE_VERTEX vertex = {0};
                vertex.TextureCoord0.u = ((float)textureCoords[tris[t].index_st[v]].t / (float)header->skinheight);
                vertex.TextureCoord0.t = ((float)textureCoords[tris[t].index_st[v]].s / (float)header->skinwidth);

                vertex.Position.x = (vertices[tris[t].index_xyz[v]].v[0] * frames->scale[0]) + frames->translate[0];
                vertex.Position.z = (vertices[tris[t].index_xyz[v]].v[1] * frames->scale[1]) + frames->translate[1];
                vertex.Position.y = (vertices[tris[t].index_xyz[v]].v[2] * frames->scale[2]) + frames->translate[2];

                // calculate normal
                vertex.Normal.x = 0;
                vertex.Normal.y = 0;
                vertex.Normal.z = 0;

                // avoid adding duplicate vertices.  If a duplicate vertex is found, just add an indice
                // for that triangle.
                bool dupeFound = false;
                for (size_t dupeV = 0; dupeV < frameData.grvertices.size(); dupeV++)
                {
                    if (memcmp(&frameData.grvertices[dupeV], &vertex, sizeof(vertex)) == 0)
                    {
                        frameData.grindices.push_back(dupeV);
                        dupeFound = true;
                        break;
                    }
                }

                if (!dupeFound)
                {
                    frameData.grindices.push_back(frameData.grvertices.size());
                    frameData.grvertices.push_back(vertex);
                }
            }
        }

        frameData.frame.IndicesCount = frameData.grindices.size();
        frameData.frame.VertexCount = frameData.grvertices.size();

        // Generate normals
        CHECKHR(GenerateNormals(frameData.grvertices.data(), frameData.grvertices.size(), frameData.grindices.data(), frameData.grindices.size()), L"Error generating normals for geometry resource");

        md2Frames.push_back(frameData);
    }
    //SaveGeometryResourceAsWaveFrontOBJ(L"x64\\debug\\debug_geometry.obj", frameData.grvertices.data(), frameData.grvertices.size(), frameData.grindices.data(), frameData.grindices.size());

    // Save geometry resource to disk

    grheader.FrameCount = md2Frames.size();

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &resourceFile), L"Error creating geometry resource file");
    CHECKHR(resourceFile->Write(&grheader, sizeof(grheader), &bytesWritten), L"Error writing geometry resource header");
    
    for (size_t i = 0; i < md2Frames.size(); i++)
    {
        CHECKHR(resourceFile->Write(&md2Frames[i].frame, sizeof(md2Frames[i].frame), &bytesWritten),L"Failed to write frame header");
        CHECKHR(resourceFile->Write(md2Frames[i].grvertices.data(), md2Frames[i].grvertices.size() * sizeof(GEOMETRY_RESOURCE_VERTEX), &bytesWritten), L"Error writing geometry resource vertices");
        CHECKHR(resourceFile->Write(md2Frames[i].grindices.data(), md2Frames[i].grindices.size() * sizeof(uint32_t), &bytesWritten), L"Error writing geometry resource indices");
    }

Exit:

    return hr;
}

HRESULT ConvertPCXToBMP(PCXFILEHEADER* header, byte_t* data, uint32_t size, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmheader, bool flipImage)
{
    DWORD bytesRead = 0;
    DWORD bytesWritten = 0;
    BITMAPINFOHEADER bmih = {0};
    bmpal.resize(256);
    LARGE_INTEGER liFileSize = {0};
    int scanlineSize = 0;
    std::vector<byte_t> scanLineTemp;

    scanlineSize = header->planes * header->bpl;
    ldiv_t divResult = ldiv(scanlineSize, sizeof(long));
    if (divResult.rem > 0)
    {
        scanlineSize = (scanlineSize/sizeof(long) + 1) * sizeof(long);
    }
        
    bmih.biSize           = sizeof(BITMAPINFOHEADER);
    bmih.biWidth          = header->x2 - header->x1 + 1;
    bmih.biHeight         = header->y2 - header->y1 + 1;
    bmih.biPlanes         = header->planes;
    bmih.biBitCount       = header->bpp;
    bmih.biCompression    = BI_RGB;
    bmih.biSizeImage      = scanlineSize * bmih.biHeight;
    bmih.biXPelsPerMeter  = 0;
    bmih.biYPelsPerMeter  = 0;
    bmih.biClrUsed        = 0;
    bmih.biClrImportant   = 0;

    bmpData.resize(bmih.biSizeImage);

    long lDataPos = 0;
    long lPos = 128;
    int iX = 0;

    for ( int iY = 0; iY < bmih.biHeight; iY++ )
    {
        // Decompress the scan line
        for ( iX = 0; iX < header->bpl; )
        {
            UINT uiValue = data[lPos++];
            if ( uiValue > 192 ) 
            {
                uiValue -= 192;  // Repeat count
                BYTE pixelColor = data[lPos++];

                if ( iX <= bmih.biWidth )
                {
                    for ( BYTE bRepeat = 0; bRepeat < uiValue; bRepeat++ )
                    {
                        bmpData[lDataPos++] = pixelColor;
                        iX++;
                    }
                }
                else
                {
                    iX += uiValue; // skip
                }
            }
            else
            {
                if ( iX <= bmih.biWidth )
                {
                    bmpData[lDataPos++] = uiValue;
                }
                iX++;
            }
        }

        // Pad the rest with zeros
        if ( iX < scanlineSize )
        {
            for ( ;iX < scanlineSize; iX++ )
            {
                bmpData[lDataPos++] = 0;
            }
        }
    }

    if (flipImage)
    {
        // Flip bitmap
        scanLineTemp.resize(bmih.biWidth);
        BYTE* pLine1 = bmpData.data();
        BYTE* pLine2 = bmpData.data() + (bmih.biSizeImage - bmih.biWidth);
        while(pLine1 <= pLine2)
        {
            CopyMemory(scanLineTemp.data(), pLine1, bmih.biWidth);
            memmove(pLine1, pLine2, bmih.biWidth);
            CopyMemory(pLine2, scanLineTemp.data(), bmih.biWidth);

            pLine1 += bmih.biWidth;
            pLine2 -= bmih.biWidth;
        }
    }

    // Write the palette
    if ( data[lPos++] == 12 )
    {
        for ( short Entry=0; Entry < (short)bmpal.size(); Entry++ )
        {
            bmpal[Entry].rgbRed       = data[lPos++];
            bmpal[Entry].rgbGreen     = data[lPos++];
            bmpal[Entry].rgbBlue      = data[lPos++];
            bmpal[Entry].rgbReserved  = 0;
        }
    }

    bmheader = bmih;

    return S_OK;
}

HRESULT GenerateNormals(GEOMETRY_RESOURCE_VERTEX* vertices, size_t numVertices, uint32_t* indices, size_t numIndices)
{
    // Copied code from Lucid Graphics system
    void*  pFirstPosition = &vertices[0].Position;
    void*  pFirstNormal   = &vertices[0].Normal;
    size_t vertexStride   = sizeof(GEOMETRY_RESOURCE_VERTEX); 
    void*  pFirstIndex    = &indices[0];
    size_t indexStride    = sizeof(uint32_t);
    bool   flipCross      = false;

    {
        bool fullSizeIndices = (indexStride >= 4);

        byte* pIndices = (byte*)pFirstIndex;
        byte* pPositions = (byte*)pFirstPosition;
        byte* pNormals = (byte*)pFirstNormal;

        uint32_t i0, i1, i2;
        DirectX::XMVECTOR v0, v1, v2, normal;

        // zero pass
        for (size_t i = 0; i < numVertices; i++)
        {
            *(DirectX::XMFLOAT3*)(pNormals + (i * vertexStride)) = DirectX::XMFLOAT3(0, 0, 0);
        }

        for (size_t i = 0; i < numIndices; i+=3)
        {
            if (fullSizeIndices)
            {
                i0 = *(uint32_t*)(pIndices + (i * indexStride));
                i1 = *(uint32_t*)(pIndices + ((i + 1) * indexStride));
                i2 = *(uint32_t*)(pIndices + ((i + 2) * indexStride));
            }
            else
            {
                i0 = (uint32_t)(*(uint16_t*)(pIndices + (i * indexStride)));
                i1 = (uint32_t)(*(uint16_t*)(pIndices + ((i + 1) * indexStride)));
                i2 = (uint32_t)(*(uint16_t*)(pIndices + ((i + 2) * indexStride)));
            }

            v0 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i0 * vertexStride)));
            v1 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i1 * vertexStride)));
            v2 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i2 * vertexStride)));

            DirectX::XMFLOAT3* n0 = (DirectX::XMFLOAT3*)(pNormals + (i0 * vertexStride));
            DirectX::XMFLOAT3* n1 = (DirectX::XMFLOAT3*)(pNormals + (i1 * vertexStride));
            DirectX::XMFLOAT3* n2 = (DirectX::XMFLOAT3*)(pNormals + (i2 * vertexStride));

            normal = flipCross ? 
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v0), DirectX::XMVectorSubtract(v1, v0))) :
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v0), DirectX::XMVectorSubtract(v2, v0)));

            DirectX::XMStoreFloat3(n0, DirectX::XMVectorAdd(XMLoadFloat3(n0), normal));
            DirectX::XMStoreFloat3(n1, DirectX::XMVectorAdd(XMLoadFloat3(n1), normal));
            DirectX::XMStoreFloat3(n2, DirectX::XMVectorAdd(XMLoadFloat3(n2), normal));
        }

        // normalize pass
        for (size_t i = 0; i < numVertices; i++)
        {
            DirectX::XMFLOAT3* n = (DirectX::XMFLOAT3*)(pNormals + (i * vertexStride));
            DirectX::XMStoreFloat3(n, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(n)));
        }
    }

    return S_OK;
}