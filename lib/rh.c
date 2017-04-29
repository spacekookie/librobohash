////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#include <robohash/rh.h>

#include "mbedtls/sha512.h"

#include "robohash/errors.h"
#include "base64.h"


#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <dirent.h>
#include <png.h>
#define RH_WARN_ERR false

// TODO: Change this! The path should come from the build script, not be hardcoded
#define RES_S1 "resources/set1/"
#define RES_S2 "resources/set2/"
#define RES_S3 "resources/set3/"
#define RES_BG1 "resources/backgrounds/bg1"
#define RES_BG2 "resources/backgrounds/bg2"
#define MAGIC_YO 3
#define CHECK_SANE if(ctx->magno != MAGIC_YO) return RH_ERR_CTX_VOID;


/** FUNCTION FORWARD DECLARATIONS **/
void transfer(unsigned char *bfr, int bfr_len, unsigned char *src);
unsigned int hash(unsigned char *str, int mod);
char *select_part_from(robohash_ctx *ctx, char *part, char *colour, int part_select);
char *select_bg_from(robohash_ctx *ctx, int set_select, int bg_select);


unsigned int robohash_init(robohash_ctx *ctx, robohash_type type, unsigned short bg, const char *salt)
{

    /** We have some functional limitations */
    if(type != RH_T_FULL) {
        fputs("ONLY FULL BODY ROBOTS ARE IMPLEMENTED YET!", stderr);
        return RH_ERR_GENERIC;
    }

    /** Make sure a valid background field is selected */
    if(!(bg == RH_BG_NONE || bg == RH_BG_ONE || bg == RH_BG_TWO)) {
        return RH_ERR_INVALID_BG;
    }

    /* Wipe the struct so we are sure it's really empty */
    memset(ctx, 0, sizeof(robohash_ctx));

    ctx->magno = MAGIC_YO;
    ctx->bfr_s = ctx->bfr_occ = 0;
    ctx->type = type;
    ctx->bg = bg;

    /** Initialise the mbedtls context */
    ctx->md_ctx = (mbedtls_sha512_context*) malloc(sizeof(mbedtls_sha512_context));
    if(ctx->md_ctx == NULL) {
        return RH_ERR_MALLOC;
    }

    mbedtls_sha512_init(ctx->md_ctx);
    return RH_ERR_OK;
}


unsigned int robohash_set_path(robohash_ctx *ctx, const char *path)
{
    CHECK_SANE

    size_t len = strlen(path);
    ctx->path = (char*) malloc(sizeof(char) * len);
    if(ctx->path == NULL) return RH_ERR_MALLOC;

    /* Copy path and return */
    strcpy(ctx->path, path);
    return 0;
}


unsigned int robohash_append_msg(robohash_ctx *ctx, const char *msg)
{
    CHECK_SANE

    size_t prev_s = ctx->bfr_s;

    /* Check that our buffer is big enough to handle new data */
    if(ctx->bfr_s < strlen(msg)) {
        if(prev_s == 0 && ctx->salt) {
            ctx->bfr_s += ((strlen(msg) + strlen((const char*) ctx->salt)) * 2);
        } else {
            ctx->bfr_s += (strlen(msg) * 2);
        }

        /* Make a new clean buffer */
        char *tmp = (char*) calloc(sizeof(char), ctx->bfr_s);

        /* Prepend the salt if it's the first block and we have one */
        if(prev_s == 0 && ctx->salt) {
            ctx->bfr_occ += strlen((const char*) ctx->salt + 2);
            strcpy(tmp, (const char*) ctx->salt);
            strcat(tmp, "::");
        }

        /* Move data from old to new */
        if(ctx->curr_bfr) strcpy(tmp, ctx->curr_bfr);

        /* Move buffer ptr and clear old */
        free(ctx->curr_bfr);
        ctx->curr_bfr = tmp;
    }

    /** Append our message and increase the occupancy counter */
    ctx->bfr_occ += strlen(msg);
    strcat(ctx->curr_bfr, msg);

    return RH_ERR_OK;
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
    const char *tmpsrc = calloc(sizeof(char), ctx->bfr_occ);
    memcpy((void*) tmpsrc, ctx->curr_bfr, ctx->bfr_occ);

    /* Then process the buffer in one go */
    mbedtls_sha512_update(ctx->md_ctx, (void*) tmpsrc, ctx->bfr_occ);

    /* Save hash output in stack array */
    char output[64];
    mbedtls_sha512_finish(ctx->md_ctx, (void*) output);

    /* Free temp buffer */
    free((void*) tmpsrc);

    char encoded[Base64encode_len(64)];
    Base64encode(encoded, output, 64);

    /* Then clear our context for the next transaction */
    free(ctx->curr_bfr);
    ctx->curr_bfr = NULL;
    ctx->bfr_occ = ctx->bfr_s = 0;

    /* Allocate the result struct */
    (*buffer) = (robohash_result*) calloc(sizeof(robohash_result),1 );
    if((*buffer) == NULL) {
        return RH_ERR_MALLOC;
    }

    /* Figure out how many hash snippets we need */
    int parts;
    switch (ctx->type) {
        default:
        case RH_T_FULL: parts = 6; break;
        case RH_T_MSTR: parts = 7; break;
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

    /* Choosing components from sub-hashes */
    int bg;
    int colours = 10;
    if(ctx->bg == RH_BG_ONE) bg = 12;
    else if(ctx->bg == RH_BG_TWO) bg = 6;

    /* Buffers for resources */
    char *mouth_res;
    char *eyes_res;
    char *acc_res;
    char *body_res;
    char *face_res;
    char *bg_res;

    if(ctx->type == RH_T_FULL) {
        int mouths = 10, eyes = 10, accessories = 10, bodies = 10, faces = 10;
        unsigned char data[blk_s];

        /* MOUTH OF THE ROBOT */
        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, (unsigned char*) snippets[0]);
        int mouth = hash(data, mouths);

        /* EYES OF THE ROBOT */
        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, snippets[1]);
        int eye = hash(data, eyes);

        /* FACE OF THE ROBOT */
        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, snippets[2]);
        int face = hash(data, faces);

        /* ACCESSORY OF THE ROBOT */
        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, snippets[3]);
        int accessory = hash(data, accessories);

        /* BODY OF THE ROBOT */
        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, snippets[4]);
        int body = hash(data, bodies);

        /* DEFINE BACKGROUND IF REQUIRED */
        int background;
        if(ctx->bg != RH_BG_NONE) {
            memset(data, 0, sizeof(unsigned char) * blk_s);
            transfer(data, blk_s, snippets[5]);
            background = hash(data, bg);
        }

        memset(data, 0, sizeof(unsigned char) * blk_s);
        transfer(data, blk_s, snippets[6]);
        int colour = hash(data, colours);

        char *colour_sel[] = { "blue", "brown", "green", "grey", "orange",
                               "pink", "purple", "red", "white", "yellow" };

        /* Select parts from collection */
        mouth_res = select_part_from(ctx, "000#Mouth", colour_sel[colour], mouth);
        eyes_res = select_part_from(ctx, "001#Eyes", colour_sel[colour], eye);
        acc_res = select_part_from(ctx, "002#Accessory", colour_sel[colour], accessory);
        body_res = select_part_from(ctx, "003#01Body", colour_sel[colour], body);
        face_res = select_part_from(ctx, "004#02Face", colour_sel[colour], face);

        switch(ctx->bg) {
            case RH_BG_ONE: bg_res = select_bg_from(ctx, 1, background); break;
            case RH_BG_TWO: bg_res = select_bg_from(ctx, 2, background); break;
            default: break;
        }

    } else {
        return RH_ERR_NOT_IMPL;
    }

    if(ctx->blind) {
        (*buffer)->blind = true;
        (*buffer)->mouth_res = mouth_res;
        (*buffer)->eyes_res = eyes_res;
        (*buffer)->acc_res = acc_res;
        (*buffer)->body_res = body_res;
        (*buffer)->face_res = face_res;
        (*buffer)->bg_res = bg_res;

    } else {
        // TODO: Generate png here
    }

    return RH_ERR_OK;
}


const char *robohash_read_buffer(robohash_ctx *ctx)
{
    /* Manually check for "magic" number */
    if(ctx->magno != MAGIC_YO) return "(INVALID)";
    return ctx->curr_bfr;
}


const char *robohash_err_v(unsigned int errno)
{
// Makes writing the error string a bit nicer
#define END "\n"

    /** Just switch through all the errors that we know of and return a message */
    switch(errno) {
        case RH_ERR_OK:         return "Operation completed successfully!" END;
        case RH_ERR_INVALID_MD: return "Invalid message digest function selected!" END;
        case RH_ERR_CTX_VOID:   return "Non initialised robohash context provided!" END;
        case RH_ERR_MALLOC:     return "Allocating memory on the heap failed!" END;
        case RH_ERR_RESOURCES:  return "Required image resources not found!" END;
        case RH_ERR_NOT_IMPL:   return "Feature missing in current implementation!" END;
        default:                return "An unknown error occured!" END;
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


/**
 *
 * PRIVATE UTILITY FUNCTIONS BELOW
 *
 */


/**
 * A numeric hash function that yields a kinda okay distributions
 * of values used to associate crypto-hashes with robo parts
 *
 * @param str
 * @param mod
 * @return
 */
unsigned int hash(unsigned char *str, int mod)
{
    int c;
    unsigned int hash = 5381; // My second favourite prime number
    while (c = *str++) hash = ((hash << 5) + hash) + c;
    return hash % mod;
}


/**
 * Why does this exist? Shouldn't that just be strcpy?
 *
 * @param bfr
 * @param bfr_len
 * @param src
 */
void transfer(unsigned char *bfr, int bfr_len, unsigned char *src) {
    for(int i = 0; i < bfr_len; i++) {
        bfr[i] = src[i];
    }
}


char *select_part_from(robohash_ctx *ctx, char *part, char *colour, int part_select)
{
    if(ctx->path == NULL) return NULL;

    DIR *dir;
    struct dirent *ent;

    /** Create space for the path*/
    char path[512];
    memset(path, 0, sizeof(path));

    /** Concat the set, color and part */
    strcpy(path, ctx->path);
    strcat(path, "/");
    strcat(path, RES_S1);
    strcat(path, colour);
    strcat(path, "/");
    strcat(path, part);
    strcat(path, "/");

    int filec = 0;
    char selected[64];
    memset(selected, 0, sizeof(selected));

    if ((dir = opendir(path)) != NULL) {
        while((ent = readdir (dir)) != NULL) {
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            if(filec == part_select) {
                strcpy(selected, ent->d_name);
                break;
            }
            filec++;
        }
        closedir(dir);
    } else {
        printf("FAILED TO OPEN DIRECTORY (%s)...\n", path);
    }

    size_t path_length = strlen(path) + strlen(selected) + 1;
    char *sel_path = (char*) calloc(sizeof(char), path_length);
    strcpy(sel_path, path);
    strcat(sel_path, selected);
    return sel_path;
}


char *select_bg_from(robohash_ctx *ctx, int set_select, int bg_select)
{
    return "";
}