////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#include "robohash.h"
#include "errors.h"

#include <memory.h>
#include <malloc.h>

#define CHECK_SANE \
    if(ctx->magno != 3) return RH_ERR_CTX_VOID;

unsigned int robohash_init(robohash_ctx *ctx, unsigned short hash_function, const char *salt)
{
    memset(ctx, 0, sizeof(robohash_ctx));

    if(hash_function != RH_MD_SHA512) {
        return RH_ERR_INVALID_MD;
    }

    /* When we're sure we can apply it, apply it! */
    ctx->hash = hash_function;

    if(salt) {
        ctx->salt = (char*) malloc(sizeof(char) * strlen(salt));
        strcpy(ctx->salt, salt);

    } else {
        ctx->salt = (char*) malloc(sizeof(char) * 1);
        strcpy(ctx->salt, "\0");
    }

    ctx->magno = 3;
    ctx->bfr_s = ctx->bfr_occ = 0;

    /** Initialise the mbedtls context */
    ctx->md_ctx = (mbedtls_sha512_context*) malloc(sizeof(mbedtls_sha512_context));
    mbedtls_sha512_init(ctx->md_ctx);

    return RH_ERR_OK;
}

unsigned int robohash_append_msg(robohash_ctx *ctx, const char *msg)
{
    CHECK_SANE

    /* Check that our buffer is big enough to handle new data */
    if(ctx->bfr_s < strlen(msg)) {
        ctx->bfr_s += (strlen(msg) * 2);
        char *tmp = (char*) malloc(sizeof(char) *  ctx->bfr_s);
        memset(tmp, 0, ctx->bfr_s);

        /* Move data */
        if(ctx->curr_bfr) strcpy(tmp, ctx->curr_bfr);

        /* Now move the buffer */
        free(ctx->curr_bfr);
        ctx->curr_bfr = tmp;
    }

    /** Append our message and increase the occupancy counter */
    ctx->bfr_occ += strlen(msg);
    strcat(ctx->curr_bfr, msg);
}

unsigned int robohash_blindness(robohash_ctx *ctx, bool blind)
{
    CHECK_SANE
    ctx->blind = blind;
    return RH_ERR_OK;
}

unsigned int robohash_build(robohash_ctx *ctx, robohash_result **buffer)
{
    CHECK_SANE

    /** Just assume that we're running on 64-bit hardware (for now) */
    mbedtls_sha512_starts(ctx->md_ctx, 0);

    /* Cut down buffer size to required only */
    const char *tmpsrc = malloc(sizeof(char) * ctx->bfr_occ);
    memcpy(tmpsrc, ctx->curr_bfr, ctx->bfr_occ);

    /* Then process the buffer in one go */
    mbedtls_sha512_update(ctx->md_ctx, tmpsrc, ctx->bfr_occ);

    /* Save hash output in stack array */
    char output[64];
    mbedtls_sha512_finish(ctx->md_ctx, output);

    /* Then clear our context for the next transaction */
    free(ctx->curr_bfr);
    ctx->bfr_occ = ctx->bfr_s = 0;

    /* Allocate the result struct */
    (*buffer) = (robohash_result*) calloc(sizeof(robohash_result),1 );
    if((*buffer) == NULL) {
        return RH_ERR_MALLOC;
    }

    (*buffer)->width = 256;
    (*buffer)->height = 256;

    /* Associate digest sections with picture elements */

    return RH_ERR_OK;
}

const char *robohash_read_buffer(robohash_ctx *ctx)
{
    CHECK_SANE

    /* Check for magic number */
    if(ctx->magno != 3) return RH_ERR_CTX_VOID;
    return ctx->curr_bfr;
}

const char *robohash_err_v(unsigned int errno)
{
    switch(errno) {
        case RH_ERR_OK:
            return "Operation completed successfully!\n";
        case RH_ERR_INVALID_MD:
            return "Invalid message digest function selected!\n";
        case RH_ERR_CTX_VOID:
            return "Non initialised robohash context provided!\n";
        case RH_ERR_MALLOC:
            return "Allocating memory on the heap failed!\n";
        case RH_ERR_RESOURCES:
            return "Required image resources not found!\n";
        default:
            return "An unknown error occured!\n";
    }
}

unsigned int robohash_free(robohash_ctx *ctx)
{
    CHECK_SANE

    /* Free memory and hash backend */
    free(ctx->curr_bfr);
    free(ctx->salt);
    mbedtls_sha512_free(ctx->md_ctx);

    /* Forcibly write over it */
    memset(ctx, 0, sizeof(robohash_ctx));
    return RH_ERR_OK;
}
