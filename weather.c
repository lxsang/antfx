#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "weather.h"
#include "utils.h"
#include "log.h"

#define MAX_CURL_PAGE_LENGTH 2048

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
    antfx_wt_t* wt = (antfx_wt_t*) data;
    char buffer[MAX_CURL_PAGE_LENGTH];
    float temperature;
    char* url;
    regex_t regex;
    regmatch_t matches[2];
    antfx_conf_t* conf = antfx_get_config();
    int ret;
    LOG("Fetching weather infomation");
    CURL *curl_handle;
    CURLcode res;
    (void)memset(buffer, 0, MAX_CURL_PAGE_LENGTH);
    curl_handle = curl_easy_init();
    snprintf(buffer, MAX_CURL_PAGE_LENGTH, conf->weather_api_uri, conf->fav.city);
    curl_easy_setopt(curl_handle, CURLOPT_URL, buffer);
    (void)memset(buffer, 0, MAX_CURL_PAGE_LENGTH);
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

    memset(wt->desc, '\0', sizeof(wt->desc));
    memcpy(wt->desc, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    strcat(wt->desc, " C, ");
    // description
    if (!regex_match("\\s*\"main\":\\s*\"([a-zA-Z]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather description value");
        return NULL;
    }
    memcpy(wt->desc + strlen(wt->desc), buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    LOG("Weather: %s", wt->desc);
    
    // icons
    if (!regex_match("\\s*\"icon\":\\s*\"([a-zA-Z0-9]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather icon value");
        return NULL;
    }

    memset(wt->icon, '\0', sizeof(wt->icon));
    memcpy(wt->icon, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    LOG("Icon: '%s'", wt->icon);
    wt->update = 1;
}
void weather_update(antfx_wt_t* wt)
{
    pthread_t th;
    if (pthread_create(&th, NULL, weather_thread_handler, wt) != 0)
    {

        ERROR("Error creating weather thread: %s", strerror(errno));
        return;
    }

    if (pthread_detach(th) != 0)
    {
        ERROR("Unable to detach weather thread: %s", strerror(errno));
    }
}