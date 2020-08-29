#ifndef WEATHER_H
#define WEATHER_H
#include <curl/curl.h>

#define WT_MAX_DESC 255

typedef struct {
    char desc[WT_MAX_DESC];
    char icon[10];
    int update;
} antfx_wt_t;

void weather_update(antfx_wt_t*);
#endif