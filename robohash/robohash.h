////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef _ROBOHASH_H_
#define _ROBOHASH_H_

#define RH_MD_SHA512 1111
#define RH_BG_NONE   2222
#define RH_BG_ONE    2224
#define RH_BG_TWO    2228

/** This is used to make it nicer to interact with finished picture */
#include <unitypes.h>
#include <unistring/stdbool.h>
#include <stdio.h>
#include "mbedtls/sha512.h"

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
} robohash_result;

typedef enum {
    RH_T_FULL, RH_T_MSTR, RH_T_HEAD
} robohash_type;

typedef struct
{
    unsigned short          hash;
    mbedtls_sha512_context  *md_ctx;
    unsigned char           *salt;
    char                    *curr_bfr;
    size_t                  bfr_s, bfr_occ;
    short                   magno;

    bool                    bg, blind;
    robohash_type           type;
} robohash_ctx;

/**
 * Used to initialise a robohash context for future hashes.
 * Takes the following parameters:
 * 
 *   ctx:             An empty context either on stack or heap to
 *                        write information into how to hash.
 *   hash_function:   A short from RH_HASH_* to use in the backend
 *   salt:            Optional, a string to use as salt for all 
 *                        future operations.
 */
unsigned int robohash_init(robohash_ctx *ctx, robohash_type type, unsigned short bg, const char *salt);

/**
 * Tell librobohash to not return a bitmap but an array of resources for the user to assemble
 */
unsigned int robohash_blindness(robohash_ctx *ctx, bool blind);

/**
 * Appends a piece of data (possibly string to a buffer that's kept inside
 * the robohash context. This context is then evaluated when calling
 * build.
 *
 * @param ctx: Active and initialised context
 * @param msg: A piece of a message or data
 */
unsigned int robohash_append_msg(robohash_ctx *ctx, const char *msg);

const char *robohash_read_buffer(robohash_ctx *ctx);

/**
 *
 */
unsigned int robohash_build(robohash_ctx *ctx, robohash_result *(*buffer));

/**
 * Not yet implemented!
 */
unsigned int robohash_verify(robohash_ctx *ctx, void *buffer_a, void *buffer_b);

/**
 * Returns a string that describes a status code in librobohash
 */
const char *robohash_err_v(unsigned int errno);

/**
 * Cleans up allocated memory and devaluates the hash context
 */
unsigned int robohash_free(robohash_ctx *ctx);

#endif // _ROBOHASH_H_