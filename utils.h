#ifndef UTILS_H
#define UTILS_H
#include <regex.h>
#define UNUSED(c) (void)(c)

int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches);
void trim(char *str, const char delim);
#endif