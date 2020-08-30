
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "db.h"
#include "log.h"
#include "weather.h"
#include "conf.h"


#define BUFFLEN 1024
#define DEFAULT_CITY "Paris"
#define DEFAULT_M_PATH "/media/music/"

static int antfx_db_query(void* user, int (*call_back)(void *, int, char **, char **), const char *fstring, ...)
{
    char buffer[BUFFLEN];
    sqlite3 *db;
    va_list arguments;
    int ret;
    char *err_msg = 0;
    antfx_conf_t* config = antfx_get_config();
    va_start(arguments, fstring);
    vsnprintf(buffer, BUFFLEN, fstring, arguments);
    va_end(arguments);
    LOG("QUERY: %s", buffer);
    ret = sqlite3_open(config->db_path, &db);
    if (ret != SQLITE_OK)
    {
        ERROR("Cannot open database: %s %s", config->db_path, sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    sqlite3_mutex_enter(sqlite3_db_mutex(db));
    ret = sqlite3_exec(db, buffer, call_back, user, &err_msg);
    sqlite3_mutex_leave(sqlite3_db_mutex(db));
    if (ret != SQLITE_OK)
    {
        ERROR("Cannot query : %s", err_msg);
        sqlite3_free(err_msg);
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    sqlite3_close(db);
    return ret;
}

static int antfx_db_fm_cb(void *user, int count, char **data, char **columns)
{
    antfx_fm_record_t* record = (antfx_fm_record_t*) malloc(sizeof(antfx_fm_record_t));
    (void) columns;
    void** user_data =  (void**) user;
    void (*fn)(antfx_fm_record_t*,void*) = (void (*)(antfx_fm_record_t*,void*)) user_data[0];
    if(count != 3)
    {
        ERROR("Number column in returned data is not correct: %d expected 3", count);
        return -1;
    }
    record->id = atoi(data[0]);
    record->freq = atof(data[2]);
    strncpy(record->name, data[1], ANTFX_MAX_STR_BUFF_SZ);
    fn(record,user_data[1]);
    return 0;
}

static int antfx_db_fav_cb(void *user, int count, char **data, char **columns)
{
    antfx_user_fav_t* fav = (antfx_user_fav_t*) user;
    if(count != 4)
    {
        ERROR("Number column in returned data is not correct: %d expected 3", count);
        return -1;
    }
    fav->id = atoi(data[0]);
    strncpy(fav->city,data[1], ANTFX_MAX_STR_BUFF_SZ);
    fav->shuffle = atoi(data[2]);
    strncpy(fav->music_path,data[3], ANTFX_MAX_STR_BUFF_SZ);
    return 0;
}

int antdfx_db_init()
{
    if(antfx_db_query(NULL,NULL, RADIO_TABLE_SQL) == -1)
    {
        ERROR("Unable to create table: radio");
        return -1;
    }
    if(antfx_db_query(NULL,NULL, ALARM_TABLE_SQL) == -1)
    {
        ERROR("Unable to create table: alarm");
        return -1;
    }
    if(antfx_db_query(NULL,NULL, FAV_TABLE_SQL) == -1)
    {
        ERROR("Unable to create table: fav");
        return -1;
    }
    return 0;
}
int antfx_db_get_alarm(antfx_alarm_t *conf)
{
    return -1;
}
int antfx_db_save_alarm(antfx_alarm_t *conf)
{
    return -1;
}
int antfx_db_add_fm_channel(antfx_fm_record_t *record)
{
    int ret;
    ret = antfx_db_query(NULL, NULL, "INSERT INTO radio(name,frequency) VALUES ('%s',%.2f)",record->name, record->freq);
    return ret;
}
int antfx_db_rm_fm_channel(antfx_fm_record_t* r)
{
    int ret;
    if(r)
    {
        ret = antfx_db_query(NULL, NULL, "DELETE FROM radio WHERE id=%d",r->id);
        if(ret == 0)
        {
            free(r);
        }
        return ret;
    }
    return 0;
}
int antfx_db_fetch_fm_channels(void (*callback)(antfx_fm_record_t *, void*), void* user)
{
    int ret;
    void * data[2];
    data[0] = callback;
    data[1] = user;
    ret = antfx_db_query( data, antfx_db_fm_cb, "SELECT * FROM radio");
    return ret;
}

int antfx_db_get_fav(antfx_user_fav_t* conf)
{
    strncpy(conf->city, DEFAULT_CITY, sizeof(conf->city));
    strncpy(conf->music_path, DEFAULT_M_PATH, sizeof(conf->music_path));
    conf->shuffle = 0;
    conf->id = 0;
    return antfx_db_query(conf, antfx_db_fav_cb,"SELECT * FROM fav LIMIT 1");
}
int antfx_db_save_fav(int reload)
{
    int ret;
    antfx_conf_t* conf = antfx_get_config();
    if(conf->fav.id == 0)
    {
        ret = antfx_db_query(
            NULL,
            NULL,
            "INSERT INTO fav(city,shuffle,music,input,output) VALUES ('%s',%d, '%s',NULL,NULL)",
            conf->fav.city, conf->fav.shuffle, conf->fav.music_path);
    }
    else
    {
        ret = antfx_db_query(
            NULL,
            NULL,
            "UPDATE fav SET city='%s',shuffle=%d,music='%s',input='%s',output='%s' WHERE id=%d",
            conf->fav.city, conf->fav.shuffle, conf->fav.music_path, conf->fav.id, conf->fav.input,conf->fav.output);
    }
    if(ret != -1 && reload)
    {
        ret = antfx_db_get_fav(&conf->fav);
        if(ret != -1)
        {
            weather_update(&conf->weather);
        }
    }
    return ret;
}