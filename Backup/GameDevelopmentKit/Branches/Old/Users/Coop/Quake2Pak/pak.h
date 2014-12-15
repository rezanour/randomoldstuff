#pragma once

#pragma pack(push,1)

struct PAK_HEADER
{
    char signature[4]; 
    int dirOffset;
    int dirLength;
};

struct PAK_DIRECTORY_ENTRY
{
    char filename[56];
    int offset;
    int length;
};

struct MD2_HEADER
{
    int ident;      // magic number. must be equal to "IDP2"
    int version;    // md2 version. must be equal to 8

    int skinwidth;  // width of the texture
    int skinheight; // height of the texture
    int framesize;  // size of one frame in bytes

    int num_skins;  // number of textures
    int num_xyz;    // number of vertices
    int num_st;     // number of texture coordinates
    int num_tris;   // number of triangles
    int num_glcmds; // number of opengl commands
    int num_frames; // total number of frames

    int ofs_skins;  // offset to skin names (64 bytes each)
    int ofs_st;     // offset to s-t texture coordinates
    int ofs_tris;   // offset to triangles
    int ofs_frames; // offset to frame data
    int ofs_glcmds; // offset to opengl commands
    int ofs_end;    // offset to end of file
};

struct MD2_VERTEX
{
    byte_t v[3];                // compressed vertex (x, y, z) coordinates
    byte_t lightnormalindex;    // index to a normal vector for the lighting
};

struct MD2_TEXTURE_COORDINATE
{
    short s;
    short t;
};

struct MD2_ANIMATION_FRAME
{
    float      scale[3];       // scale values
    float      translate[3];   // translation vector
    char       name[16];       // frame name
    MD2_VERTEX verts[1];       // first vertex of this frame
};

struct MD2_TRIANGLE
{
    short index_xyz[3]; // indexes to triangle's vertices
    short index_st[3];  // indexes to vertices' texture coorinates
};

#pragma pack(pop)