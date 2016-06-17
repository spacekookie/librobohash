////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef _ROBOHASH_H_
#define _ROBOHASH_H_

#define RH_MD_SHA256 0xA
#define RH_MD_SHA512 0xB
#define RH_MD_TIGER2 0xC
#define RH_MD_BLAKE 0xD

/** This is used to make it nicer to interact with finished picture */
#include <unitypes.h>
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

typedef struct
{
    unsigned short          hash;
    mbedtls_sha512_context  *md_ctx;
    unsigned char           *salt;
    char                    *curr_bfr;
    size_t                  bfr_s, bfr_occ;
    short                   magno;
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
unsigned int robohash_init(robohash_ctx *ctx, unsigned short hash_function, const char *salt);

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
 *
 */
unsigned int robohash_verify(robohash_ctx *ctx, void *buffer_a, void *buffer_b);

/**
 *
 */
const char *robohash_err_v(unsigned int errno);

/**
 *
 */
unsigned int robohash_free(robohash_ctx *ctx);

#endif // _ROBOHASH_H_