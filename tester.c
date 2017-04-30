#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <robohash/rh.h>
#include <robohash/imgur.h>

const char words[][6] = {"argon", "emina", "fogey", "balch", "molto", "china", "pross", "bream", "laban", "baker",
                         "fifty", "hakea", "gawra", "doria", "bloke", "lenes", "buret", "luffa", "kirov", "liker",
                         "nikos", "hazen", "mammy", "mures", "saxon", "zayin", "janie", "favor", "cotte", "cacao",
                         "knish", "ammon", "hemin", "kadai", "kondo", "brent", "brogh", "podgy", "cream", "meryl",
                         "lipid", "adela", "dylan", "marys", "pokie", "hooey", "brute", "angie", "linda", "femur",
                         "alvis", "vomit", "power", "mulla", "havel", "flory", "byrne", "snuck", "wavey", "burke",
                         "poloi", "elath", "eanes", "testy", "fecit", "hiker", "pasay", "purin", "sidon", "latex",
                         "ghazi", "swish", "arvol", "tripe", "stalk", "torii", "whose", "spade", "atila", "titan",
                         "ginzo", "jolty", "alamo", "dried", "rover", "foram", "loewe", "crest", "tanis", "unsad",
                         "riser", "starr", "ccuta", "percy", "flake", "balas", "putto", "slide", "fayme", "bejel",
                         "boffo", "chook", "nikon", "spiel", "aviso", "parsi", "apsis", "enfin", "lucca", "infix",
                         "marin", "posen", "vitus", "jorum", "nanny", "rowte", "ceiba", "glary", "siple", "abbai",
                         "lucan", "akita", "tuart", "nonet", "might", "amuse", "mushy", "pskov", "tonic", "sunay",
                         "aimak", "acton", "doorn", "hodur", "laker", "varna", "thant", "vapid", "kirby", "jedda",
                         "hiker", "legit", "dozen", "holst", "lowse", "tonus", "vinie", "obuda", "never", "fenny",
                         "ahwaz", "claim", "storm", "abeam", "ellin", "since", "creed", "quant", "beria", "rowel",
                         "kinau", "irish", "yakut", "presa", "fulah", "piura", "ampyx", "korma", "alary", "pluto",
                         "canal", "erose", "pesky", "maker", "ascus", "tobol", "heald", "punka", "nanna", "nikos",
                         "colum", "trews", "going", "fusee", "visor", "haulm", "peter", "hoots", "jesus", "ruler",
                         "twite", "admin", "fluid", "agura", "skete", "speel", "basin", "thokk", "footy", "unfar"};

#define PATH "/home/spacekookie/Projects/code/librobohash/"

void do_hashing(const char *word)
{
    /* Store state */
    int ret;
    robohash_ctx ctx;
    printf("=== RESULT '%s' ===\n", word);

    /** Initialise librobohash */
    ret = robohash_init(&ctx, RH_T_FULL, RH_BG_ONE);
    printf("Init: %s", robohash_err_v(ret));
    if (ret != 0) exit(ret);

    /* Set the resource path properly */
    robohash_set_path(&ctx, PATH);

    /* Override blind mode */
    robohash_blindness(&ctx, true);

    /** Append a message */
    robohash_append_msg(&ctx, word);

    /** Then read back the buffer to us (let's say we're paranoid) */
    char buffer[256];
    memset(buffer, 0, 256);
    strcpy(buffer, robohash_read_buffer(&ctx));
    printf("Now in buffer: '%s'\n", buffer);

    /** Now actually build the result */
    robohash_result *result;
    ret = robohash_build(&ctx, &result);
    printf("Building: %s", robohash_err_v(ret));
    if (ret != 0) exit(ret);

    /** Print all of the assembled pieces */
    printf("Mouth: %s\n", result->mouth_res);
    printf("Eyes: %s\n", result->eyes_res);
    printf("Acc: %s\n", result->acc_res);
    printf("Body: %s\n", result->body_res);
    printf("Face: %s\n", result->face_res);
    printf("Background: %s\n", result->bg_res);

#define RESOLUTION 300
    imgur_img *bg, *body, *face, *eyes, *mouth, *acc;
    rh_imgur_alloc(&bg, RESOLUTION, RESOLUTION);
    rh_imgur_alloc(&body, RESOLUTION, RESOLUTION);
    rh_imgur_alloc(&face, RESOLUTION, RESOLUTION);
    rh_imgur_alloc(&eyes, RESOLUTION, RESOLUTION);
    rh_imgur_alloc(&mouth, RESOLUTION, RESOLUTION);
    rh_imgur_alloc(&acc, RESOLUTION, RESOLUTION);

    ret = rh_imgur_loadff(bg, result->bg_res);
    printf("Loading background: %s", robohash_err_v(ret));

    rh_imgur_loadff(body, result->body_res);
    rh_imgur_loadff(face, result->face_res);
    rh_imgur_loadff(eyes, result->eyes_res);
    rh_imgur_loadff(mouth, result->mouth_res);
    rh_imgur_loadff(acc, result->acc_res);

    rh_imgur_merge(bg, body);
    rh_imgur_merge(bg, face);
    rh_imgur_merge(bg, eyes);
    rh_imgur_merge(bg, mouth);
    rh_imgur_merge(bg, acc);

    rh_imgur_storepng(bg, "/home/spacekookie/segfault.png");

    /** Free those strings TODO: Write function for that */
    free(result->mouth_res);
    free(result->eyes_res);
    free(result->acc_res);
    free(result->body_res);
    free(result->face_res);
    free(result);

    /** Free the rest of the context */
    ret = robohash_free(&ctx);
    printf("Free: %s", robohash_err_v(ret));
    if (ret != 0) exit(ret);
    printf("=== RESULT '%s' ===\n\n", word);
}

int main(void)
{
    printf("Length: %i\n", (int) sizeof("RGBA"));


//    for(int i = 0; i < 1; i++) {
    do_hashing("SEGFAULT");
//    }

    return 0;
}