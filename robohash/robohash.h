////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////


#define RH_MD_SHA256 0xA
#define RH_MD_SHA512 0xB
#define RH_MD_TIGER2 0xC
#define RH_MD_BLAKE 0xD

/** This is used to make it nicer to interact with finished picture */
#define robohash_result void

typedef struct
{
  unsigned short      hash;
  mbed_ctr_seed       *seed;

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
unsigned int robohash_init(robohash_ctx *ctx, unsigned short hash_function, unsigned char *salt);

/**
 *
 */
unsigned int robohash_build(robohash_ctx *ctx, void *buffer);

/**
 *
 */
unsigned int robohash_verify(robohash_ctx *ctx, void *buffer_a, void *buffer_b);

/**
 *
 */
const char *robothash_err_v(unsigned int errno);

/**
 *
 */
unsigned int robohash_free(robohash_ctx *ctx);
