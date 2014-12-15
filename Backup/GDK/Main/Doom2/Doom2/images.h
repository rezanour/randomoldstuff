#ifndef _IMAGES_H_
#define _IMAGES_H_

//
// IMAGES
//
// Methods for loading images, building textures from patches, etc...
//

BOOL ImagesInitialize();
void ImagesUninitialize();

//
// SPRITES
//
// Sprites in DOOM are images that represent "things". In the simplest case, sprites
// are just a single, transparent supporting image which represents something like an ammo clip.
//
// In other scenarios, primarily around enemies, sprites can contain several frames of animation
// and can represent the object from various angles. The format of the NAME of the sprite is this:
//
// First 4 characters represent an identifier which tells you what this is. Ex: SARG, TROO, SKULL, SKEL, etc...
// Next char is the frame of animation that this sprite goes to, starting with A and going through the alphabet.
// Next char is a number 1->8 which represents the direction. 1 is head on, 2 - > 8 represent 45 deg increments clockwise
// The last two chars are optional, and repeat the pattern of the previous 2 (frame + direction). When used in this manner, the sprite is flipped horizontally.
//
// Examples:
//    SARGA1 -> SARG + A + 1 = SARG character, 0th frame (A), facing directly at you (1)
//    TROOA2A8 -> TROO + A + 2 + A + 8 = TROO character, 0th frame (A), facing either 2 or 8 (mirrored horiz when used for 8)
//
//
// sprite format
//
// each sprite is loaded as 8 bytes of header data (width, height, left, and top).
// following that is an array of column offsets (each 4 byte integers). The length of this
// array is equal to the width of the image, since there is one column per pixel width of the image.
//
// The Data pointer points to the beginning of the entire sprite (at the beginning of header). This is
// because the offsets are relative to this. The format of each column is as follows:
//
// a column is made up of 'posts', which are the column equivalent of spans when talking about rows. A post
// is a sequence of pixels contained in a column. Transparency is handled by skipping those pixels using multiple
// posts. Here's an example:
//
// imagine you have a column that has 5 pixels of empty space, then 10 pixels (of varying colors), then 2 spaces of empty space,
// then 17 pixels of data.
//
// This would be encoded as 2 posts. The first has a rowstart of 5 (meaning it starts 5 pixels down from the top of the image),
// it would have a pixelCount of 10, and then a buffer of 10 pixels containing the color data (as indices into palette of course).
//
// The next post would have a rowstart of 17 (5 empty + 10 colors + 2 more empty), and a pixelCount of 17, followed by the color data for 
// those 17 pixels.
//
// The rowstart & pixelcount are stored as bytes, and there is a byte of padding after pixelcount (before colordata) and one more after the last post in the column.
// The posts end when you find one with a rowcount of (byte)-1 == 255. This is the signal that the column is complete.
//
// postdata summary:
//    byte rowstart;
//    byte pixelcount;
//    byte padding;
//    (byte * pixelcount) pixels
//
//  after last one for the column (255 rowstart), one more byte padding.
//

#pragma pack(push, 1)
typedef struct
{
    // DO NOT reorder or move these.
    // this is the header block as read directly from lump!
    short Width;
    short Height;
    short Left;
    short Top;

    // this you can reorder to your heart's content :)

    int ColumnOffsets[8]; // size is actually [width]
} sprite_t;

#pragma pack(pop)


//
// FLATS
//
// flats are 64x64, non transparent images. This includes floor and ceiling tiles mostly.
// these are the simplest format. Just read 4096 (64x64) bytes which is the pixel data. That's it.
//

#define FLAT_WIDTH 64
#define FLAT_HEIGHT 64


//
// TEXTURES
//
// textures are composite images made up of patches, which are sets of sprites.
// The first time a texture is loaded, it will load all the patches necessary and
// cache the sprites so they are ready to use.
//

#pragma pack(push, 1)

typedef struct
{
    short xOffset;
    short yOffset;
    resource_t Sprite;
} texturesprite_t;

typedef struct
{
    char Name[NAME_SIZE];
    short Width;
    short Height;
    short NumSprites;
    texturesprite_t Sprites[1]; // size is NumSprites
} texture_t;

#pragma pack(pop)


//
// IMAGES methods
//

// Load a sprite given it's lump name
sprite_t* ImagesLoadSprite(const char* name);

// Free a sprite
void ImagesFreeSprite(sprite_t* sprite);

// Load a flat given it's lump name
byte_t* ImagesLoadFlat(const char* name);

// Free a flat
void ImagesFreeFlat(byte_t* flat);

// load/composite a texture
texture_t* ImagesLoadTexture(const char* name);

void ImagesFreeTexture(texture_t* texture);



#endif // _IMAGES_H_