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


/** Forward declare so we can keep the crypto ~private~ */
typedef struct mbedtls_sha512_context sha512_context;


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
 *
 */
unsigned int robohash_init(robohash_ctx *ctx, robohash_type type, unsigned short bg);


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
 * Build a robo from what was allocated in the buffer and store it in a result
 * struct that is returned to the user.
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
 *
 * @param ctx
 * @return
 */
unsigned int robohash_free(robohash_ctx *ctx);


/*********
 *
 * SOME COOL (or not) UTILITY FUNCTIONS BELOW
 *
 ************/


/**
 * Read back the buffer that we already allocated in the context. Not
 * really sure why you would want that. But hey, it's a neat little
 * utility function
 *
 * @param ctx
 * @return
 */
const char *robohash_read_buffer(robohash_ctx *ctx);


/**
 * Small utility function that gets the current path (on POSIX compliant
 * operating systems - yes, I'm looking at you windows). If this function
 * doesn't work for you, try to fix it and give me a PR?
 *
 * It's supposed to make the finding of resources easier when they're not globally
 * installed somewhere easy or you want to package them with your application.
 *
 * @return current project path
 */
char *robohash_get_currdir();


/**
 * Set the resource path. This is absolutely important if you actually care
 * about finding the robot pictures. FIND THE ROBOTS GOD FUCKING DAMN IT!
 *
 * @param ctx
 * @param path
 * @return
 */
unsigned int robohash_set_path(robohash_ctx *ctx, const char *path);


/**
 * Tell librobohash to not return a bitmap but only the array of resources
 * for the user to assemble themselves. This could be useful for...idk something.
 *
 * @param ctx
 * @param blind
 * @return
 */
unsigned int robohash_blindness(robohash_ctx *ctx, bool blind);


/**
 * Returns a string message (in english) about what type of error you got!
 *
 * @param errno
 * @return
 */
const char *robohash_err_v(unsigned int errno);


#endif // _ROBOHASH_H_