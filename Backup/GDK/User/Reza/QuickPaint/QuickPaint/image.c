#include "common.h"

//typedef struct
//{
//    surface_t* surface;
//    short xOffset;
//    short yOffset;
//} layer_t;
//
//typedef struct
//{
//    short width;
//    short height;
//    byte_t bpp;
//
//    short numLayers;
//    short maxLayers;
//    layer_t* layers;
//} image_t;
//
//
//image_t* DGCreateImage(short width, short height, byte_t bpp);
//void DGDestroyImage(image_t* image);
//
//
////
//// public image methods
////
//
//handle_t DGCreateImageObject(short width, short height, byte_t bpp)
//{
//    image_t* image = DGCreateImage(width, height, bpp);
//
//    if (!image)
//    {
//        return INVALID_HANDLE;
//    }
//
//    return DGCreateObject(OBJECT_IMAGE, image, (pfnObjectDeleter)DGDestroyImage);
//}
//
//void DGGetImageInfo(handle_t image, short* width, short* height, byte_t* bpp, short* numLayers)
//{
//    image_t* img = (image_t*)DGDataFromObject(OBJECT_IMAGE, image);
//    if (img)
//    {
//        *width = img->width;
//        *height = img->height;
//        *bpp = img->bpp;
//        *numLayers = img->numLayers;
//    }
//    else
//    {
//        *width = *height = *numLayers = -1;
//        *bpp = 0;
//    }
//}
//
//short DGGetImageWidth(handle_t image)
//{
//    image_t* img = (image_t*)DGDataFromObject(OBJECT_IMAGE, image);
//    return img ? img->width : -1;
//}
//
//short DGGetImageHeight(handle_t image)
//{
//    image_t* img = (image_t*)DGDataFromObject(OBJECT_IMAGE, image);
//    return img ? img->height : -1;
//}
//
//byte_t DGGetImageBpp(handle_t image)
//{
//    image_t* img = (image_t*)DGDataFromObject(OBJECT_IMAGE, image);
//    return img ? img->bpp : 0;
//}
//
//short DGGetImageNumLayers(handle_t image)
//{
//    image_t* img = (image_t*)DGDataFromObject(OBJECT_IMAGE, image);
//    return img ? img->numLayers : -1;
//}
//
//
////
//// internal image methods
////
//
//image_t* DGCreateImage(short width, short height, byte_t bpp)
//{
//    image_t* image;
//
//    // validate parameters
//    if (width <= 0 || height <= 0 || !DGIsValidBpp(bpp))
//    {
//        DGDebugOut("invalid image parameters (%d, %d, %d).\n", width, height, bpp);
//        return NULL;
//    }
//
//    // allocate image structure
//    image = (image_t*)DGAlloc("image header", sizeof(image_t));
//    if (!image)
//    {
//        DGDebugOut("Failed to allocate image.\n");
//        return NULL;
//    }
//
//    // fill image structure
//    image->width = width;
//    image->height = height;
//    image->bpp = bpp;
//
//    // allocate layer list
//    image->maxLayers = 10;
//    image->layers = (layer_t*)DGAlloc("image layer list", sizeof(layer_t) * image->maxLayers);
//    if (!image->layers)
//    {
//        DGDebugOut("Failed to allocate layer list for image.\n");
//        DGFree(image);
//        return NULL;
//    }
//
//    // create initial layer
//    image->layers[0].xOffset = 0;
//    image->layers[0].yOffset = 0;
//    image->layers[0].surface = DGCreateSurface(width, height, bpp);
//    if (!image->layers[0].surface)
//    {
//        DGDebugOut("Failed to create initial layer for image.\n");
//        DGFree(image->layers);
//        DGFree(image);
//        return NULL;
//    }
//
//    image->numLayers = 1;
//
//    return image;
//}
//
//void DGDestroyImage(image_t* image)
//{
//    assert(image);
//    if (image)
//    {
//        short i;
//        for (i = 0; i < image->numLayers; ++i)
//        {
//            DGDestroySurface(image->layers[i].surface);
//        }
//
//        DGFree(image->layers);
//        DGFree(image);
//    }
//}
