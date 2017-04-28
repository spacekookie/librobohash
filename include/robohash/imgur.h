////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef ROBOHASH_IMGUR_H
#define ROBOHASH_IMGUR_H

typedef struct imgur_png {
    int width, height;
    void **data;
} imgur_png;


/**
 * Allocates an empty image struct according to the size provided. The data
 * padding will be in-line with what libpng expects to store it's images.
 *
 * @param w
 * @param h
 * @param img
 * @return
 */
int rh_imgur_alloc(int w, int h, imgur_png **img);


/**
 * Loads a png from disk and stores it's data in the previously allocated
 * struct that needs to be initialised in order for this to work.
 *
 * Will throw an error if the allocated resolution doesn't line up with the
 * image resolution.
 *
 * @param img
 * @param path
 * @return
 */
int rh_imgur_load(imgur_png *img, const char *path);


/**
 * Merges an image onto a base, overwriting parts of the base where the img
 * doesn't have alpha255 components.
 *
 * @param base
 * @param img
 * @return
 */
int rh_imgur_merge(imgur_png *base, imgur_png *img);


/**
 * Takes an initialised png struct and stores it to disk into a new png file.
 * This is the last step in librobohash after which the user can then use the
 * image in their application easily.
 *
 * @param img
 * @param path
 * @return
 */
int rh_imgur_store(imgur_png *img, const char *path);

#endif
