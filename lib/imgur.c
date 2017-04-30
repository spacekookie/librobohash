/* librobohash components */
#include <robohash/errors.h>
#include <robohash/imgur.h>

#include "farbfeld/ff.h"

/* stdlib imports */
#include <malloc.h>
#include <memory.h>
#include <netinet/in.h>

#define LEN(x) (sizeof (x) / sizeof *(x))


int rh_imgur_alloc(imgur_img **img, uint32_t width, uint32_t height)
{
    (*img) = (imgur_img*) malloc(sizeof(imgur_img));
    if(*img == NULL) return RH_ERR_MALLOC;
    memset((*img), 0, sizeof(imgur_img));

    /* Assign the width x height */
    (*img)->width = width;
    (*img)->height = height;

    /* Allocate enough space for our image */
    size_t img_size = sizeof(uint16_t) * width * height * 4;
    (*img)->data = (uint16_t*) malloc(img_size);
    memset((*img)->data, 0, img_size);

    /* Check that the malloc worked - potentially free */
    if((*img)->data == NULL) {
        free(*img);
        return RH_ERR_MALLOC;
    }

    /* Ah ah ah, you didn't say the magic word */
    memcpy((*img)->magic, "farbfeld", 8);

    /* Well that went well :) */
    return RH_ERR_HUGESUCCESS;
}


int rh_imgur_merge(imgur_img *base, imgur_img *layer)
{
    /* Make sure both images exists */
    if(base == NULL || layer == NULL) return RH_ERR_NULL_IMG;

    /* Make sure resolutions line up (roughly) */
    if(base->width < layer->width || base->height < layer->height) return RH_ERR_RESOLUTION_MISMATCH;

    uint32_t i;
    uint16_t rgba[4];

    /** Iterate 1 pixel at a time */
    for(i = 0; i < layer->width * layer->height * 4; i += 4) {
        memcpy(rgba, &layer->data[i], sizeof(rgba));

        /** Is the pixel transparent or otherwise empty? */
        if(rgba[3] == 0 || (rgba[2] == 0 && rgba[1] == 0 && rgba[0] == 0))
            continue;

        /** Copy pixel to the base image buffer */
        memcpy(&base->data[i], rgba, sizeof(rgba));
    }

    return RH_ERR_HUGESUCCESS;
}


int rh_imgur_loadff(imgur_img *img, const char *path)
{
    uint32_t i, ret, width, height;
    uint32_t header[4];
    FILE *fp;

    /** Open our ff file and read the header */
    fp = fopen(path, "rb");
    fread(header, sizeof(uint32_t), 4, fp);

    /** Did you say the magic word? */
    if(memcmp("farbfeld", header, 8) != 0) return RH_ERR_FILE_OPEN_FAILED;

    /** Read the resolution */
    width = ntohl(header[2]);
    height = ntohl(header[3]);

    /** Make sure the data will actually fit into memory */
    if(img->width != width || img->height != height) return RH_ERR_RESOLUTION_MISMATCH;

    /** Continue loading data */
    fread(img->data, sizeof(uint16_t), width * height * 4, fp);

    return RH_ERR_HUGESUCCESS;
}


int rh_imgur_storepng(imgur_img *img, const char *path)
{
    if (img == NULL || path == NULL) return RH_ERR_NULL_IMG;

//#ifdef __ROBOHASH_FF_2_PNG__
    return store_as_png(img, path);
//#else
//    return RH_ERR_NOT_IMPL;
//#endif
}
