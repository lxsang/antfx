#ifndef DB_H
#define DB_H

#include "default.h"

#define RADIO_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS radio (\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
name TEXT NOT NULL, \
frequency NUMERIC );"

#define ALARM_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS alarm (\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
timestamp INTEGER,\
type INTEGER,\
source TEXT);"

#define FAV_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS fav (\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
city TEXT,\
shuffle INTEGER,\
music TEXT,\
input INTEGER,\
output INTEGER,\
input_volume INTEGER,\
output_volume INTEGER);"

typedef enum {
    U_FM_FREQ,
    U_MUSIC_NAME
} antfx_user_data_type_t;

typedef struct {
    int id;
    char name[ANTFX_MAX_STR_BUFF_SZ];
    float freq;
    void* user_data;
} antfx_fm_record_t;

typedef struct {
    int id;
    char city[ANTFX_MAX_STR_BUFF_SZ];
    int shuffle;
    char music_path[ANTFX_MAX_STR_BUFF_SZ];
    int input;
    int output;
    int input_volume;
    int output_volume;
} antfx_user_fav_t;

typedef union {
    float freq;
    char music[ANTFX_MAX_STR_BUFF_SZ];
} antfx_user_data_t;

typedef struct {
    int id;
    int timestamp;
    antfx_user_data_type_t type;
    antfx_user_data_t udata;
} antfx_alarm_t;

int antdfx_db_init();
int antfx_db_get_alarm(antfx_alarm_t*);
int antfx_db_save_alarm(antfx_alarm_t*);
int antfx_db_add_fm_channel(antfx_fm_record_t*);
int antfx_db_rm_fm_channel(antfx_fm_record_t*);
int antfx_db_fetch_fm_channels(void (*)(antfx_fm_record_t*, void*), void* );
int antfx_db_get_fav(antfx_user_fav_t*);
int antfx_db_save_fav(int);
#endif