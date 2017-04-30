#ifndef ROBOHASH_UTIL_H
#define ROBOHASH_UTIL_H

#include <stdint.h>
#include <stdio.h>

#define __ROBOHASH_FF_2_PNG__

extern char *argv0;

#define LEN(x) (sizeof (x) / sizeof *(x))

void ff_read_header(uint32_t *width, uint32_t *height);
void ff_write_header(uint32_t width, uint32_t height);

int parse_mask(const char *, uint16_t mask[3]);

int fshut(FILE *, const char *);

#undef reallocarray
void *reallocarray(void *, size_t, size_t);
void *ereallocarray(void *optr, size_t nmemb, size_t size);

#undef strtonum
long long strtonum(const char *, long long, long long, const char **);
long long estrtonum(const char *, long long, long long);

#endif //ROBOHASH_UTIL_H
