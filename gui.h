#ifndef GUI_H
#define GUI_H

#include "antfx.h"
#include "conf.h"

void antfx_ui_update();
void antfx_ui_update_weather(const char*);
void antfx_ui_update_weather_icon(const char*);
void antfx_ui_update_status(const char*);
void antfx_ui_main(engine_config_t conf);

#endif