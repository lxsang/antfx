#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
#include <curl/curl.h>
#include <regex.h>
#include "conf.h"
#include "gui.h"

#define MAX_CURL_PAGE_LENGTH 2048


static int running = 0;
static antfx_conf_t g_config;


void stop(int sig)
{
    UNUSED(sig);
    running = 0;
}

static void init_hw_clock();
static void fm_set_freq(double f);
static void fm_mute();
static double fm_get_freq();
static void weather_update();
static void *weather_thread_handler(void *data);
static int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches);
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

static int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int ret;
    /* Compile regular expression */
    reti = regcomp(&regex, expr, REG_ICASE | REG_EXTENDED);
    if (reti)
    {
        //ERROR("Could not compile regex: %s",expr);
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        ERROR("Regex match failed: %s", msgbuf);
        //return 0;
    }

    /* Execute regular expression */
    reti = regexec(&regex, search, msize, matches, 0);
    if (!reti)
    {
        //LOG("Match");
        ret = 1;
    }
    else if (reti == REG_NOMATCH)
    {
        //LOG("No match");
        ret = 0;
    }
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        ERROR("Regex match failed: %s", msgbuf);
        ret = 0;
    }

    regfree(&regex);
    return ret;
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
    curl_easy_setopt(curl_handle, CURLOPT_URL, g_config.weather_api_uri);
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
    int ret;

    if(argc == 2)
    {
        ret = antfx_read_config(argv[1], &g_config);
        if(ret == -1)
        {
            LOG("Unable to read config file: %s. Try to read default config file: %s", argv[1], DEFAULT_CONF);
            ret = antfx_read_config(DEFAULT_CONF, &g_config);
        }
    }
    else
    {
        ret = antfx_read_config(DEFAULT_CONF, &g_config);
    }

    if(ret == -1)
    {
        ERROR("Unable to read config file. Quit.");
        exit(1);
    }
    conf.default_w = 480;
    conf.default_h = 320;
    conf.defaut_bbp = 16;
    conf.dev = g_config.fb_dev;
    conf.tdev = g_config.ts_dev;
    time_t last_weather_check = 0;
    time_t now;
    UNUSED(ret = system(g_config.startup_cmd));
    curl_global_init(CURL_GLOBAL_ALL);
    // start the hardware clock
    init_hw_clock();
    // start display engine
    antfx_ui_main(conf);
    antfx_ui_update_location(g_config.location);
    fm_set_freq(94.1); // RFM
    running = 1;
    while (running)
    {
        now = time(NULL);
        if (difftime(now, last_weather_check) > g_config.weather_check_period)
        {
            weather_update();
            last_weather_check = now;
        }
        antfx_ui_update_datetime();
        lv_task_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    fm_mute();
    antfx_release();
    curl_global_cleanup();
}

static void init_hw_clock()
{
    int fd;
    char buf[32];
    int ret;
    fd = open(g_config.i2c_dev_del, O_WRONLY);
    if (fd != -1)
    {
        snprintf(buf, 32, "0x%02X", g_config.i2c_hw_clock_addr);
        ret = write(fd, buf, strlen(buf));
        close(fd);
    }
    fd = open(g_config.i2c_dev_new, O_WRONLY);
    if (fd == -1)
    {
        ERROR("Unable to open sys file for registering HW clock: %s", strerror(errno));
        return;
    }
    snprintf(buf, 32, "%s 0x%02X", "ds1307", g_config.i2c_hw_clock_addr);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret != (int)strlen(buf))
    {
        ERROR("Unable to write command to init HW clock");
        return;
    }
    usleep(1000000);
    ret = system("hwclock -s");
    if (ret == -1)
    {
        ERROR("Unable to execute commad for applying datetime from HW clock: %s", strerror(errno));
        return;
    }
}
static void fm_set_freq(double f)
{
    uint8_t radio[5] = {0};
    char buff[32];
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    freq_b = 4 * (f * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_l = freq_b & 0XFF;

    //printf ("Frequency = "); printf("%f",frequency);
    //printf("\n"); // data to be sent

    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(g_config.i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c set freq command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO on at frequency: %f", f);
    snprintf(buff, 32, "FM: %.2f Mhz", f);
    antfx_ui_update_status(buff);
}
static void fm_mute()
{
    uint8_t radio[5] = {0};
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    double frequency = fm_get_freq();
    if (frequency == -1)
    {
        return;
    }
    freq_b = 4 * (frequency * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_h = freq_h | 0x80; // mutes the radio
    freq_l = freq_b & 0XFF;

    //load the above into the array
    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(g_config.i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c mute command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO off at frequency: %f", frequency);
    antfx_ui_update_status("");
}
static double fm_get_freq()
{
    uint8_t radio[5] = {0};
    ssize_t ret;
    int fd;
    double frequency;

    if ((fd = wiringPiI2CSetup(g_config.i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = read(fd, radio, 5);
    close(fd);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to read i2c command from radio control: %s", strerror(errno));
        return -1;
    }

    frequency = ((((radio[0] & 0x3F) << 8) + radio[1]) * 32768 / 4 - 225000) / 10000;
    frequency = round(frequency * 10.0) / 1000.0;
    frequency = round(frequency * 10.0) / 10.0;

    return frequency;
}