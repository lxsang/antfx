#ifndef DB_H
#define DB_H

#define DB_MAX_TEXT_SIZE 255


#define RADIO_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS \"radio\" (\
\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,\
\"name\" TEXT NOT NULL, \
\"frequency\" NUMERIC );"

#define ALARM_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS \"alarm\" (\
\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,\
\"timestamp\" INTEGER,\
\"type\" INTEGER,\
\"source\" TEXT);"

#define FAV_TABLE_SQL  "\
CREATE TABLE IF NOT EXISTS \"fav\" (\
\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,\
\"city\" TEXT,\
\"shuffle\" INTEGER,\
\"music\" TEXT);"

typedef enum {
    U_FM_FREQ,
    U_MUSIC_NAME
} antfx_user_data_type_t;

typedef struct {
    int id;
    char name[DB_MAX_TEXT_SIZE];
    float freq;
    void* user_data;
} antfx_fm_record_t;

typedef struct {
    int id;
    char city[DB_MAX_TEXT_SIZE];
    int shuffle;
     char music_path[DB_MAX_TEXT_SIZE];
} antfx_user_fav_t;

typedef union {
    float freq;
    char music[DB_MAX_TEXT_SIZE];
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
int antfx_db_save_fav(antfx_user_fav_t*);
#endif