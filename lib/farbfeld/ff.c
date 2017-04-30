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
