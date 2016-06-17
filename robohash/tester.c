#include <stdlib.h>
#include <robohash.h>
#include <stdio.h>
#include "robohash.h"

int main(void)
{
    int ret;
    robohash_ctx ctx;
    ret = robohash_init(&ctx, RH_T_FULL, RH_BG_ONE, NULL);
    printf("Init: %s", robohash_err_v(ret));
    if(ret != 0) exit(ret);

    robohash_append_msg(&ctx, "Hello ");
    robohash_append_msg(&ctx, "this ");
    robohash_append_msg(&ctx, "is ");
    robohash_append_msg(&ctx, "cool");

    const char *bfr = robohash_read_buffer(&ctx);
    printf("Current buffer: '%s'\n", bfr);

    /* Now actually build the result */
    robohash_result *result;
    ret = robohash_build(&ctx, &result);
    printf("Building: %s", robohash_err_v(ret));
    if(ret != 0) exit(ret);

    ret = robohash_free(&ctx);
    printf("Free: %s", robohash_err_v(ret));
    if(ret != 0) exit(ret);

    return 0;
}