////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#include "robohash.h"
#include "errors.h"
#include "base64.h"


#include <memory.h>
#include <malloc.h>
#include <png.h>
#include <stdlib.h>

#define RH_WARN_ERR false

#define CHECK_SANE \
    if(ctx->magno != 3) return RH_ERR_CTX_VOID;

unsigned int robohash_init(robohash_ctx *ctx, robohash_type type, unsigned short bg, const char *salt)
{
    memset(ctx, 0, sizeof(robohash_ctx));

    if(!(bg == RH_BG_NONE || bg == RH_BG_ONE || bg == RH_BG_TWO)) {
        return RH_ERR_INVALID_BG;
    }

    if(salt) {
        fputs("DON'T USE SALTS! THEY ARE BROKEN!", stderr);
        return RH_ERR_GENERIC;
        ctx->salt = (char*) malloc(sizeof(char) * strlen(salt));
        strcpy(ctx->salt, salt);
    }

    ctx->magno = 3;
    ctx->bfr_s = ctx->bfr_occ = 0;
    ctx->type = type;
    ctx->bg = bg;

    /** Initialise the mbedtls context */
    ctx->md_ctx = (mbedtls_sha512_context*) malloc(sizeof(mbedtls_sha512_context));
    mbedtls_sha512_init(ctx->md_ctx);

    return RH_ERR_OK;
}

unsigned int robohash_append_msg(robohash_ctx *ctx, const char *msg)
{
    CHECK_SANE

    int prev_s = ctx->bfr_s;

    /* Check that our buffer is big enough to handle new data */
    if(ctx->bfr_s < strlen(msg)) {
        if(prev_s == 0 && ctx->salt) {
            ctx->bfr_s += ((strlen(msg) + strlen(ctx->salt)) * 2);
        } else {
            ctx->bfr_s += (strlen(msg) * 2);
        }
        char *tmp = (char*) malloc(sizeof(char) *  ctx->bfr_s);
        memset(tmp, 0, ctx->bfr_s);

        /* Move data */
        if(ctx->curr_bfr) strcpy(tmp, ctx->curr_bfr);

        /* Append a salt if it's the first block and we have one */
        if(prev_s == 0 && ctx->salt) {
            ctx->bfr_occ += strlen(ctx->salt + 2);
            strcpy(tmp, ctx->salt);
            strcat(tmp, "::");
        }

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

    char encoded[Base64encode_len(64)];
    Base64encode(encoded, output, 64);

    // printf("Encoded: %s\n", encoded, strlen(encoded));

    /* Then clear our context for the next transaction */
    free(ctx->curr_bfr);
    ctx->curr_bfr = NULL;
    ctx->bfr_occ = ctx->bfr_s = 0;

    /* Allocate the result struct */
    (*buffer) = (robohash_result*) calloc(sizeof(robohash_result),1 );
    if((*buffer) == NULL) {
        return RH_ERR_MALLOC;
    }

//    (*buffer)->width = 256;
//    (*buffer)->height = 256;

    /* Figure out how many hash snippets we need */
    int parts;
    switch (ctx->type) {
        case RH_T_FULL: parts = 5; break;
        case RH_T_MSTR: parts = 6; break;
    }
    if(ctx->bg != RH_BG_NONE) parts += 1;

    /* Cut hash apart */
    int it = 0;                         // Iterator for the loop
    int blk_s = sizeof(encoded) / parts; // Size of a block
    int curr_start = 0;       // Start and end points for cut
    char snippets[parts][blk_s];        // Map of snippets

    /* Prepare array and cut snippets */
    memset(snippets, 0, sizeof(char) * parts * blk_s);
    for(it = 0; it < parts; it++) {
        memcpy(&snippets[it], &encoded[curr_start], blk_s * sizeof(char));
        curr_start += blk_s;
    }

    memcpy((*buffer)->msg_dig, snippets, sizeof(char) * blk_s * parts);
    return RH_ERR_OK;

//    /* Choosing components from sub-hashes */
//    int colours = 10;
//    int bg;
//    if(ctx->bg == RH_BG_ONE) bg = 12;
//    else if(ctx->bg = RH_BG_TWO) bg = 6;
//
//    if(ctx->type == RH_T_FULL) {
//        int mouths, eyes, accessory, body, face = 9;
//
//
//        int mouth = ((int) snippets[1]) % mouths;
//
//    } else {
//        return RH_ERR_NOT_IMPL;
//    }

//    if(ctx->blind) {
//        return RH_ERR_OK;
//    }
//
//    return RH_ERR_OK;
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
        case RH_ERR_NOT_IMPL:
            return "Feature missing in current implementation!\n";
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

// Debug stuff
//    it = 0;
//    for(it; it < parts; it++) {
//        int a = 0;
//        printf(">> ");
//        for(a; a < blk_s; a++) {
//            printf("%c", snippets[it][a]);
//        }
//        printf("\n");
//    }