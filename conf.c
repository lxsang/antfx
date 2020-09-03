#include <string.h>
#include <stdlib.h>
#include "ini/ini.h"
#include "conf.h"
#include "log.h"

#define EQU(a, b) (strcmp(a, b) == 0)

static int ini_handle(void *user_data, const char *section, const char *name,
                      const char *value)
{
    antfx_conf_t *config = (antfx_conf_t *)user_data;
    if (EQU(section, "antfxd") && EQU(name, "db_path"))
    {
        strncpy(config->db_path, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "display") && EQU(name, "fb_dev"))
    {
        strncpy(config->display_dev.fb_dev, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "display") && EQU(name, "ts_dev"))
    {
        strncpy(config->display_dev.ts_dev, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "i2cdev") && EQU(name, "i2c_dev_del"))
    {
        strncpy(config->i2c_dev.dev_del, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "i2cdev") && EQU(name, "i2c_dev_new"))
    {
        strncpy(config->i2c_dev.dev_new, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "i2cdev") && EQU(name, "i2c_hw_clock_addr"))
    {
        config->i2c_dev.clock_addr = (uint8_t)strtol(value, NULL, 0);
    }
    else if (EQU(section, "i2cdev") && EQU(name, "i2c_hw_radio_addr"))
    {
        config->i2c_dev.radio_addr = (uint8_t)strtol(value, NULL, 0);
    }
    else if (EQU(section, "display") && EQU(name, "ts_calibrate_cmd"))
    {
        strncpy(config->display_dev.ts_calibrate_cmd, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "display") && EQU(name, "ts_calibrate_file"))
    {
        strncpy(config->display_dev.ts_calibrate_file, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_api_uri"))
    {
        strncpy(config->weather.weather_api_uri, value, ANTFX_MAX_STR_BUFF_SZ);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_check_period"))
    {
        config->weather.weather_check_period = (int)atoi(value);
    }
    else
    {
        ERROR("Unknow config: [%s] %s = %s", section, name, value);
    }
    return 1;
}

int antfx_read_config(const char *file, antfx_conf_t *config)
{
    memset(config->db_path, 0, ANTFX_MAX_STR_BUFF_SZ);

    memset(config->display_dev.fb_dev, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->display_dev.ts_calibrate_cmd, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->display_dev.ts_calibrate_file, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->display_dev.ts_dev, 0, ANTFX_MAX_STR_BUFF_SZ);

    memset(config->fav.city, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->fav.music_path, 0, ANTFX_MAX_STR_BUFF_SZ);
    config->fav.id = 0;
    config->fav.shuffle = 0;
    config->fav.input = DEFAULT_INPUT_DEV;
    config->fav.output = DEFAULT_OUTPUT_DEV;
    config->fav.input_volume = DEFAULT_INPUT_VOLUME;
    config->fav.output_volume = DEFAULT_OUTPUT_VOLUME;

    memset(config->i2c_dev.dev_del, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->i2c_dev.dev_new, 0, ANTFX_MAX_STR_BUFF_SZ);
    config->i2c_dev.clock_addr = 0;
    config->i2c_dev.radio_addr = 0;

    memset(config->weather.desc, 0, ANTFX_MAX_STR_BUFF_SZ);
    memset(config->weather.icon, 0, 10);
    memset(config->weather.weather_api_uri, 0, ANTFX_MAX_STR_BUFF_SZ);
    config->weather.weather_check_period = 300;
    config->weather.update = 0;

    LOG("Read config file: %s", file);
    if (ini_parse(file, ini_handle, config) < 0)
    {
        ERROR("Can't load '%s'", file);
        return -1;
    }
    if (antdfx_db_init() == -1)
    {
        ERROR("Unable to int database: %s", config->db_path);
        return -1;
    }
    if (antfx_db_get_fav(&config->fav) == -1)
    {
        ERROR("Can't load user config from database");
    }
    return 0;
}