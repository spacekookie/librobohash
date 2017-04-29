#ifndef ROBOHASH_FF_H
#define ROBOHASH_FF_H

#include <robohash/imgur.h>

/**
 *
 * @param img
 * @param filename
 * @return
 */
int store_as_png(imgur_img *img, const char *filename);


/**
 *
 * @param img
 * @param filename
 * @return
 */
int load_from_png(imgur_img *img, const char *filename);

#endif //ROBOHASH_FF_H
