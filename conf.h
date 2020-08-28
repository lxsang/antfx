#ifndef CONF_H
#define CONF_H
#include "db.h"

#define DEFAULT_CONF "/etc/antfxd/config.ini"
#define MAX_CONF_SIZE 255
typedef struct
{
    char db_path[MAX_CONF_SIZE];
    char fb_dev[MAX_CONF_SIZE];
    char ts_dev[MAX_CONF_SIZE];
    char i2c_dev_del[MAX_CONF_SIZE];
    char i2c_dev_new[MAX_CONF_SIZE];
    char ts_calibrate_cmd[MAX_CONF_SIZE];
    char ts_calibrate_file[MAX_CONF_SIZE];
    unsigned int i2c_hw_clock_addr;
    unsigned int i2c_hw_radio_addr;
    char weather_api_uri[MAX_CONF_SIZE];
    int weather_check_period;
    char startup_sound[MAX_CONF_SIZE];
    antfx_user_fav_t fav;
} antfx_conf_t;

int antfx_read_config(const char*, antfx_conf_t*);
#endif