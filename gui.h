#ifndef GUI_H
#define GUI_H

#include "antfx.h"

typedef struct
{
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *lbl_weather;
    lv_obj_t *lbl_status;
    lv_obj_t *lbl_location;
    lv_obj_t *lbl_weather_img;
} antfx_screen_info_t;


void antfx_ui_show_calendar(lv_obj_t * obj, lv_event_t event);
void antfx_ui_close_popup(lv_obj_t * obj, lv_event_t event);
void antfx_ui_update_datetime();
void antfx_ui_update_location(const char*);
void antfx_ui_update_weather(const char*);
void antfx_ui_update_weather_icon(const char*);
void antfx_ui_update_status(const char*);
void antfx_ui_main(engine_config_t conf);
#endif