#ifndef CONF_H
#define CONF_H
#include <stdint.h>
#include "db.h"
#include "weather.h"
#include "hw.h"
#include "media.h"
#include "default.h"

typedef struct {
    char fb_dev[ANTFX_MAX_STR_BUFF_SZ];
    char ts_dev[ANTFX_MAX_STR_BUFF_SZ];
    char ts_calibrate_cmd[ANTFX_MAX_STR_BUFF_SZ];
    char ts_calibrate_file[ANTFX_MAX_STR_BUFF_SZ];
} antfx_display_dev_t;


typedef struct
{
    antfx_i2cdev_t i2c_dev;
    antfx_display_dev_t display_dev;
    antfx_audio_t audio;
    antfx_user_fav_t fav;
    antfx_wt_t weather;
    antfx_media_ctl_t media;
    char db_path[ANTFX_MAX_STR_BUFF_SZ];
} antfx_conf_t;

int antfx_read_config(const char*, antfx_conf_t*);
antfx_conf_t *antfx_get_config();
#endif