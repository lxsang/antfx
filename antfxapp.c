#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <pthread.h>
#include <curl/curl.h>
#include "conf.h"
#include "gui.h"
#include "db.h"
#include "utils.h"
#include "hw.h"
#include "media.h"

#define MAX_CURL_PAGE_LENGTH 2048

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


static void weather_update();
static void *weather_thread_handler(void *data);
static size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *buff);

static size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int dlen = size * nmemb;
    int total_len = 0;
    char *buffer = (char *)stream;
    total_len = strlen(buffer) + dlen;
    if (total_len > MAX_CURL_PAGE_LENGTH - 1)
    {
        ERROR("Buffer overflow: page data is too long");
        return -1;
    }
    (void)memcpy(buffer + strlen(buffer), ptr, dlen);
    buffer[total_len] = '\0';
    return total_len;
}

static void *weather_thread_handler(void *data)
{
    UNUSED(data);
    char buffer[MAX_CURL_PAGE_LENGTH];
    char x_text[64];
    float temperature;
    regex_t regex;
    regmatch_t matches[2];
    int ret;
    LOG("Fetching weather infomation");
    CURL *curl_handle;
    CURLcode res;
    (void)memset(buffer, 0, MAX_CURL_PAGE_LENGTH);
    curl_handle = curl_easy_init();
    sprintf(buffer, MAX_CURL_PAGE_LENGTH, g_config.weather_api_uri, g_config.fav.city);
    curl_easy_setopt(curl_handle, CURLOPT_URL, buffer);
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, buffer);

    res = curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    if (res != CURLE_OK)
    {
        ERROR("Unable to fetch weather from url: %s", curl_easy_strerror(res));
        return NULL;
    }
    // now parse the temperature
    if (!regex_match("\\s*\"temp\":\\s*([0-9.]+),?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match temperature value");
        return NULL;
    }

    memset(x_text, '\0', sizeof(x_text));
    memcpy(x_text, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    strcat(x_text, " C, ");

    // description
    if (!regex_match("\\s*\"main\":\\s*\"([a-zA-Z]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather description value");
        return NULL;
    }
    memcpy(x_text + strlen(x_text), buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    antfx_ui_update_weather(x_text);

    // icons
    if (!regex_match("\\s*\"icon\":\\s*\"([a-zA-Z0-9]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather icon value");
        return NULL;
    }

    memset(x_text, '\0', sizeof(x_text));
    memcpy(x_text, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    LOG("Icon: '%s'", x_text);
    // show icon
    antfx_ui_update_weather_icon(x_text);
}
static void weather_update()
{
    pthread_t th;
    if (pthread_create(&th, NULL, weather_thread_handler, NULL) != 0)
    {

        ERROR("Error creating weather thread: %s", strerror(errno));
        return;
    }

    if (pthread_detach(th) != 0)
    {
        ERROR("Unable to detach weather thread: %s", strerror(errno));
    }
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
    if(antdfx_db_init() == -1)
    {
        ERROR("Unable to int database: %s", g_config.db_path);
    }
    // verify if we should calibrate the display
    if ((stat(g_config.ts_calibrate_file, &st) == -1))
    {
        LOG("Calibrate the display");
        ret = system(g_config.ts_calibrate_cmd);
        if (ret != -1 && WEXITSTATUS(ret) != 127)
        {
            ret = open(g_config.ts_calibrate_file, O_CREAT | O_WRONLY, 0644);
            if (ret != -1)
            {
                close(ret);
            }
            else
            {
                ERROR("Unable to create calibarion file: %s", g_config.ts_calibrate_file);
            }
        }
        else
        {
            ERROR("Unable to calibrate screen, touch may not be used: %s", g_config.ts_calibrate_cmd);
        }
    }
    // init the display
    conf.default_w = 480;
    conf.default_h = 320;
    conf.defaut_bbp = 16;
    conf.dev = g_config.fb_dev;
    conf.tdev = g_config.ts_dev;
    time_t last_weather_check = 0;
    time_t now;
    anfx_music_init();
    if(antfx_music_play(g_config.startup_sound) == -1)
    {
        ERROR("Unable to play startup sound");
    }
    curl_global_init(CURL_GLOBAL_ALL);
    // start the hardware clock
    init_hw_clock();
    // start display engine
    antfx_ui_main(conf);
    antfx_ui_update_location(g_config.fav.city);
    running = 1;
    while (running)
    {
        now = time(NULL);
        if (difftime(now, last_weather_check) > g_config.weather_check_period)
        {
            weather_update();
            last_weather_check = now;
        }
        antfx_ui_update();
        lv_task_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    antfx_ui_update_status("");
    if(antfx_db_save_fav(&g_config.fav) == -1)
    {
        ERROR("Unable to save user configuration");
    }
    anfx_music_release();
    fm_mute();
    antfx_release();
    curl_global_cleanup();
}