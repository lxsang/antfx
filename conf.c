#include <string.h>
#include <stdlib.h>
#include "ini/ini.h"
#include "conf.h"
#include "log.h"

#define EQU(a,b) (strcmp(a,b) == 0)

static int ini_handle(void *user_data, const char *section, const char *name,
                      const char *value)
{
    antfx_conf_t* config = (antfx_conf_t*) user_data;
    if (EQU(section, "antfxd") && EQU(name, "db_path"))
    {
        strncpy(config->db_path,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "fb_dev"))
    {
        strncpy(config->fb_dev,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "ts_dev"))
    {
        strncpy(config->ts_dev,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "i2c_dev_del"))
    {
        strncpy(config->i2c_dev_del,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "i2c_dev_new"))
    {
        strncpy(config->i2c_dev_new,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "i2c_hw_clock_addr"))
    {
        config->i2c_hw_clock_addr = (unsigned int)strtol(value, NULL, 0);
    }
    else if (EQU(section, "hardware") && EQU(name, "i2c_hw_radio_addr"))
    {
        config->i2c_hw_radio_addr = (unsigned int)strtol(value, NULL, 0);
    }
    else if (EQU(section, "hardware") && EQU(name, "ts_calibrate_cmd"))
    {
        strncpy(config->ts_calibrate_cmd,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "hardware") && EQU(name, "ts_calibrate_file"))
    {
        strncpy(config->ts_calibrate_file,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_api_uri"))
    {
        strncpy(config->weather_api_uri,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_check_period"))
    {
        config->weather_check_period = (int) atoi(value);
    }
    else if (EQU(section, "antfxd") && EQU(name, "startup_sound"))
    {
        strncpy(config->startup_sound,value, MAX_CONF_SIZE);
    }
    else
    {
        ERROR("Unknow config: [%s] %s = %s", section, name, value);
    }
    return 1;
}

int antfx_read_config(const char* file, antfx_conf_t* config)
{
    LOG("Read config file: %s", file);
    if (ini_parse(file, ini_handle, config) < 0)
    {
        ERROR("Can't load '%s'", file);
        return -1;
    }
    if(antdfx_db_init() == -1)
    {
        ERROR("Unable to int database: %s", config->db_path);
        return -1;
    }
    if(antfx_db_get_fav(&config->fav) == -1)
    {
        ERROR("Can't load user config from database");
    }
    return 0;
}