#ifndef UTILS_H
#define UTILS_H
#include <regex.h>
#include "conf.h"
#define UNUSED(c) (void)(c)

int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches);
void trim(char* str, const char delim);
antfx_conf_t* antfx_get_config();
#endif