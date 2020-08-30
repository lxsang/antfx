#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "antfx.h"
#include "gui.h"
#include "conf.h"



static int running = 0;
static antfx_conf_t g_config;

antfx_conf_t* antfx_get_config()
{
    return &g_config;
}
static void stop(int sig)
{
    UNUSED(sig);
    running = 0;
}

int main(int argc, char *argv[])
{
    engine_config_t conf;
    LOG_INIT("antfx");
    signal(SIGPIPE, SIG_IGN);
    signal(SIGABRT, SIG_IGN);
    signal(SIGINT, stop);
    signal(SIGTERM, stop);

    struct stat st;
    int ret;
    if (argc == 2)
    {
        ret = antfx_read_config(argv[1], &g_config);
        if (ret == -1)
        {
            LOG("Unable to read config file: %s. Try to read default config file: %s", argv[1], DEFAULT_CONF);
            ret = antfx_read_config(DEFAULT_CONF, &g_config);
        }
    }
    else
    {
        ret = antfx_read_config(DEFAULT_CONF, &g_config);
    }

    if (ret == -1)
    {
        ERROR("Unable to read config file. Quit.");
        exit(1);
    }
    // verify if we should calibrate the display
    if ((stat(g_config.display_dev.ts_calibrate_file, &st) == -1))
    {
        LOG("Calibrate the display");
        ret = system(g_config.display_dev.ts_calibrate_cmd);
        if (ret != -1 && WEXITSTATUS(ret) != 127)
        {
            ret = open(g_config.display_dev.ts_calibrate_file, O_CREAT | O_WRONLY, 0644);
            if (ret != -1)
            {
                close(ret);
            }
            else
            {
                ERROR("Unable to create calibarion file: %s", g_config.display_dev.ts_calibrate_file);
            }
        }
        else
        {
            ERROR("Unable to calibrate screen, touch may not be used: %s", g_config.display_dev.ts_calibrate_cmd);
        }
    }
    // init the display
    conf.default_w = LV_HOR_RES_MAX;
    conf.default_h = LV_VER_RES_MAX;
    conf.defaut_bbp = 16;
    conf.dev = g_config.display_dev.fb_dev;
    conf.tdev = g_config.display_dev.ts_dev;
    time_t last_weather_check = 0;
    time_t now;
    anfx_music_init();
    if(antfx_music_play(g_config.audio.startup_sound) == -1)
    {
        ERROR("Unable to play startup sound");
    }
    curl_global_init(CURL_GLOBAL_ALL);
    // start the hardware clock
    init_hw_clock();
    // start display engine
    antfx_ui_main(conf);
    running = 1;
    while (running)
    {
        now = time(NULL);
        if (difftime(now, last_weather_check) > g_config.weather.weather_check_period)
        {
            weather_update(&g_config.weather);
            last_weather_check = now;
        }
        antfx_ui_update();
        lv_task_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    if(antfx_db_save_fav(0) == -1)
    {
        ERROR("Unable to save user configuration");
    }
    anfx_music_release();
    fm_mute();
    antfx_release();
    curl_global_cleanup();
}