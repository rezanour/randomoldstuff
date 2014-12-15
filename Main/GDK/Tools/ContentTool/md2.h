#pragma once
#include <stdint.h>
#include <GDK.h>

#pragma pack(push,1)

struct md2_fileheader_t
{
    int32_t ident;      // magic number. must be equal to "IDP2"
    int32_t version;    // md2 version. must be equal to 8

    int32_t skinwidth;  // width of the texture
    int32_t skinheight; // height of the texture
    int32_t framesize;  // size of one frame in bytes

    int32_t num_skins;  // number of textures
    int32_t num_xyz;    // number of vertices
    int32_t num_st;     // number of texture coordinates
    int32_t num_tris;   // number of triangles
    int32_t num_glcmds; // number of opengl commands
    int32_t num_frames; // total number of frames

    int32_t ofs_skins;  // offset to skin names (64 bytes each)
    int32_t ofs_st;     // offset to s-t texture coordinates
    int32_t ofs_tris;   // offset to triangles
    int32_t ofs_frames; // offset to frame data
    int32_t ofs_glcmds; // offset to opengl commands
    int32_t ofs_end;    // offset to end of file
};

struct md2_vertex_t
{
    uint8_t v[3];                // compressed vertex (x, y, z) coordinates
    uint8_t lightnormalindex;    // index to a normal vector for the lighting
};

struct md2_texture_coordinate_t
{
    int16_t s;
    int16_t t;
};

struct md2_animation_frame_t
{
    float        scale[3];       // scale values
    float        translate[3];   // translation vector
    int8_t       name[16];       // frame name
    md2_vertex_t verts[1];       // first vertex of this frame
};

struct md2_triangle_t
{
    int16_t index_xyz[3]; // indexes to triangle's vertices
    int16_t index_st[3];  // indexes to vertices' texture coorinates
};

#pragma pack(pop)

void ConvertMD2ToGeometryContent(_In_ std::vector<uint8_t>& md2, _Inout_ std::shared_ptr<GDK::GeometryContent>& content);