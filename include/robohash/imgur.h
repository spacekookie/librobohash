////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef ROBOHASH_IMGUR_H
#define ROBOHASH_IMGUR_H

#include <stdint.h>

/** A struct that wraps a farbfeld image */
typedef struct imgur_img {
    char magic[8];              // "farbfeld" magic number
    uint32_t width, height;     // 32bit BE resolution
    uint16_t *data;             // 4*16 BE uint [RGBA] - row-major
} imgur_img;


/**
 * Allocates a new, empty farbfeld image that can be used to build a robot
 * piece by piece. Make sure you can allocate memory on the heap for this
 * operation.
 *
 * @param img
 * @param width
 * @param height
 * @return
 */
int rh_imgur_alloc(imgur_img **img, uint32_t width, uint32_t height);


/**
 * Merges an image onto a base, overwriting parts of the base where the img
 * doesn't have alpha components.
 *
 * @param base
 * @param layer
 * @return
 */
int rh_imgur_merge(imgur_img *base, imgur_img *layer);


/**
 * Open a png image, convert it into farbfeld and load it into
 * a pre-allocated image struct. Will throw errors if the expected
 * resolution is wrong.
 *
 * @param img
 * @param path
 * @return
 */
int rh_imgur_loadpng(imgur_img *img, const char *path);


/**
 * Will take the farbfeld image data, convert it to a png and save it to
 * disk to use elsewhere.
 *
 * @param img
 * @param path
 * @return
 */
int rh_imgur_storepng(imgur_img *img, const char *path);


/**
 * Free all memory associated to an image
 *
 * @param img
 * @return
 */
int rh_imgur_free(imgur_img *img);

#endif
