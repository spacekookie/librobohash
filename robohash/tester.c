#include <stdlib.h>
#include <robohash.h>
#include <stdio.h>
#include <memory.h>
#include "robohash.h"

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

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int main(void) {
    int distro[9];

    int c;
    for (c = 0; c < 200; c++) {
        int ret;
        robohash_ctx ctx;
        ret = robohash_init(&ctx, RH_T_FULL, RH_BG_ONE, NULL);
        printf("Init: %s", robohash_err_v(ret));
        if (ret != 0) exit(ret);

        printf("Word: %s\n", words[c]);
        robohash_append_msg(&ctx, words[c]);

        robohash_result *result;
        ret = robohash_build(&ctx, &result);
        if (ret != 0){
            printf("Building: %s", robohash_err_v(ret));
            exit(ret);
        }

        for(int i = 0; i < 6; i++) {
            printf("Hash%d: ", i);

            long buf = 0;
            unsigned char *data = (unsigned char*) malloc(sizeof(char) * 14);

            for(int j = 0; j < 14; j++) {
                printf("%c", result->msg_dig[i][j]);
                data[j] = result->msg_dig[i][j];
            }
            printf(" --- ");

            buf = (unsigned int) hash(data);
            buf = (unsigned int) buf % 10;
            printf("Buf: %d\n", buf);
            distro[((unsigned int) buf)]++;
        }
        free(result);
        robohash_free(&ctx);
        printf("\n\n");
    }

    printf("==== NUMBER DISTRIBUTION ====\n");

    for(int k = 0; k < 10; k++) {
        printf("%d: %d\n", k, distro[k]);
    }

    printf("==== NUMBER DISTRIBUTION ====\n");


//
//    robohash_append_msg(&ctx, "Hello ");
//    robohash_append_msg(&ctx, "this ");
//    robohash_append_msg(&ctx, "is ");
//    robohash_append_msg(&ctx, "cool");
//
//    const char *bfr = robohash_read_buffer(&ctx);
//    printf("Current buffer: '%s'\n", bfr);
//
//    /* Now actually build the result */
//    robohash_result *result;
//    ret = robohash_build(&ctx, &result);
//    printf("Building: %s", robohash_err_v(ret));
//    if(ret != 0) exit(ret);
//
//    ret = robohash_free(&ctx);
//    printf("Free: %s", robohash_err_v(ret));
//    if(ret != 0) exit(ret);

    return 0;
}