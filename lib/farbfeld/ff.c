#include "ff.h"
#include "util.h"

#include <robohash/errors.h>

#include <stdlib.h>
#include <string.h>
#include <png.h>


int store_as_png(imgur_img *img, const char *filename)
{
    size_t rowlen = img->width * 4; // 4 uint16_t per pixel [RGBA]
    FILE *fp = NULL;
    png_struct *png_s;
    png_info *png_i;
    uint32_t i;

    /** Create a clean buffer for a row */
    uint16_t row[rowlen];
    memset(row, 0, sizeof(uint16_t) * rowlen);

    /* Open a file handle for us to use */
    fp = fopen(filename, "wb");

    /** Init some libpng shizzle */
    png_s = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_i = png_create_info_struct(png_s);
    png_init_io(png_s, fp);
    png_set_IHDR(png_s, png_i, img->width, img->height, 16, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    png_write_info(png_s, png_i);

    /** Copy one row at a time into libpng buffers */
    for(i = 0; i < img->width * img->height * 4; i += rowlen) {
        memcpy(row, &img->data[i], sizeof(uint16_t) * rowlen);
        png_write_row(png_s, (uint8_t*) row);
    }

    /** Tell libpng to get on with it already */
    png_write_end(png_s, NULL);

    /** Clean everything up and return */
    fclose(fp);
    png_free_data(png_s, png_i, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_s, NULL);

    return 0;
}


int load_from_png(imgur_img *img, const char *filename)
{
    uint32_t width, height, rowlen, r, i;
    FILE *fp = NULL;
    png_struct *png_s;
    png_info *png_i;

    uint16_t *row;
    uint8_t **pngrows;

    /** Try to open the provided file path */
    fp = fopen(filename, "rb");

    png_s = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_i = png_create_info_struct(png_s);

    png_init_io(png_s, fp);
//    if (png_get_valid(png_s, png_i, PNG_INFO_tRNS)) {
//        png_set_tRNS_to_alpha(png_s);
//    }
//    png_set_add_alpha(png_s, 255 * 257, PNG_FILLER_AFTER);
//    png_set_expand_gray_1_2_4_to_8(png_s);
//    png_set_packing(png_s);
    png_read_png(png_s, png_i, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
    width = (uint32_t) png_get_image_width(png_s, png_i);
    height = (uint32_t) png_get_image_height(png_s, png_i);
    pngrows = png_get_rows(png_s, png_i);

    size_t rows = sizeof(pngrows);

    FILE *foop = NULL;
    png_struct *png_ss;
    png_info *png_ii;

    /* Open a file handle for us to use */
    foop = fopen("/home/spacekookie/boobs.png", "wb");

    png_byte depth = png_get_bit_depth(png_s, png_i);
    printf("Colour depth: %i\n", depth);

    /** Init some libpng shizzle */
    png_ss = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_ii = png_create_info_struct(png_ss);
    png_init_io(png_ss, foop);
    png_set_IHDR(png_ss, png_ii, img->width, img->height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    png_write_info(png_ss, png_ii);

    for(r = 0; r < height; r++) {
        png_write_row(png_ss, pngrows[r]);
    }

    /** Tell libpng to get on with it already */
    png_write_end(png_ss, NULL);

    /** Clean everything up and return */
    fclose(foop);
    png_free_data(png_ss, png_ii, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ss, NULL);


    /** Now we can calculate the farbfeld rowlength */
    //rowlen = width * 4;
    //row = calloc(rowlen, sizeof(uint16_t));

    /** Write farbfeld image header */
    //img->width = width;
    //img->height = height;
    //memcpy(img->magic, "farbfeld", 8);

    /** Decode png image data */
    //switch() {
    //    case 8:
    //        for (r = 0; r < height; r++) {
    //            for (i = 0; i < rowlen; i++) {
    //                row[i] = pngrows[r][i];
    //            }
    //
    //            memcpy(&img->data[r], row, sizeof(uint16_t) * rowlen);
    //
    //            if (fwrite(row, sizeof(uint16_t), rowlen, fp) != rowlen) {
    //                return RH_ERR_FILE_OPEN_FAILED;
    //            }
    //        }
    //        break;
    //
    //    case 16:
    //        for (r = 0; r < height; ++r) {
    //            memcpy(&img->data[r], pngrows[r], sizeof(uint16_t) * rowlen);
    //
    //            if (fwrite(pngrows[r], sizeof(uint16_t), rowlen, fp) != rowlen) {
    //                return RH_ERR_FILE_OPEN_FAILED;
    //            }
    //        }
    //        break;
    //
    //    default:
    //        return 1;
    //}

    png_destroy_read_struct(&png_s, &png_i, NULL);
    return 0;
}