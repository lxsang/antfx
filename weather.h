#ifndef WEATHER_H
#define WEATHER_H
#include <curl/curl.h>
#include "default.h"

typedef struct {
    char desc[ANTFX_MAX_STR_BUFF_SZ];
    char icon[10];
    int update;
    char weather_api_uri[ANTFX_MAX_STR_BUFF_SZ];
    int weather_check_period;
} antfx_wt_t;


void weather_update(antfx_wt_t*);
#endif