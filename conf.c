#include <antd/ini.h>
#include <antd/utils.h>
#include <string.h>
#include <stdlib.h> 
#include "conf.h"
#include "log.h"

static int ini_handle(void *user_data, const char *section, const char *name,
                      const char *value)
{
    antfx_conf_t* config = (antfx_conf_t*) user_data;
    if (EQU(section, "db") && EQU(name, "db_path"))
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
    else if (EQU(section, "weather") && EQU(name, "location"))
    {
        strncpy(config->location,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_api_uri"))
    {
        strncpy(config->weather_api_uri,value, MAX_CONF_SIZE);
    }
    else if (EQU(section, "weather") && EQU(name, "weather_check_period"))
    {
        config->weather_check_period = (int) atoi(value);
    }
    else if (EQU(section, "antfxd") && EQU(name, "startup"))
    {
        strncpy(config->startup_cmd,value, MAX_CONF_SIZE);
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
    return 0;
}