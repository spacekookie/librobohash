////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef _ROBOHASH_H_
#define _ROBOHASH_H_

/** Some definitions for stuff and rather */
#define RH_MD_SHA512 1111
#define RH_BG_NONE   2222
#define RH_BG_ONE    2224
#define RH_BG_TWO    2228


/** Imports organised by length **/
#include <unistring/stdbool.h>
#include <unitypes.h>
#include <stdio.h>


typedef struct
{
    /* Parts data from blind operation */
    char    *mouth_res;
    char    *eyes_res;
    char    *acc_res;
    char    *body_res;
    char    *face_res;
    char    *bg_res;

    bool    blind;
} robohash_result;


typedef enum {
    RH_T_FULL, RH_T_MSTR, RH_T_HEAD
} robohash_type;


/** Forward declare so we can keep the crypto ~private~ */
typedef struct mbedtls_sha512_context sha512_context;


typedef struct
{
    unsigned short          hash;
    sha512_context          *md_ctx;
    unsigned char           *salt;
    char                    *curr_bfr;
    size_t                  bfr_s, bfr_occ;
    short                   magno;

    char                    *path;
    unsigned short          bg;
    bool                    blind;
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
 *
 * @param ctx
 * @param path
 * @return
 */
unsigned int robohash_set_path(robohash_ctx *ctx, const char *path);


/**
 * Tell librobohash to not return a bitmap but an array of resources
 * for the user to assemble themselves
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


/**
 *
 * @param ctx
 * @return
 */
const char *robohash_read_buffer(robohash_ctx *ctx);


/**
 *
 * @param ctx
 * @param buffer
 * @return
 */
unsigned int robohash_build(robohash_ctx *ctx, robohash_result *(*buffer));


/**
 * Not yet implemented
 *
 * @param ctx
 * @param buffer_a
 * @param buffer_b
 * @return
 */
unsigned int robohash_verify(robohash_ctx *ctx, void *buffer_a, void *buffer_b);


/**
 * Returns a string message (in english) about what type of error you got!
 *
 * @param errno
 * @return
 */
const char *robohash_err_v(unsigned int errno);


/**
 *
 * @param ctx
 * @return
 */
unsigned int robohash_free(robohash_ctx *ctx);

#endif // _ROBOHASH_H_