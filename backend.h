#ifndef ENGINE_INTERFACE
#define ENGINE_INTERFACE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lib/lvgl.h"
#include <tslib.h>
#include "log.h"

#define UNUSED(c) (void)(c)

#define AFX_EVT_NONE    0x0
#define AFX_EVT_DOWN    0x1
#define AFX_EVT_RELEASE 0x2
#define AFX_EVT_MOVE    0x03


typedef struct{
    uint8_t type;
    uint16_t data[2];
} antfx_event_t;

typedef struct{
    lv_indev_drv_t driver;
    antfx_event_t evt;
} antfx_idev_t;

/*
    all display engines need to define this interface
    the system will load them dynamically
*/
typedef struct{
    uint16_t width;
    uint16_t height;
    uint8_t bbp;
    int16_t xoffset;
    int16_t yoffset;
    uint16_t line_length;
    uint8_t* buffer;
    int handle;
    int t_handle;
    struct tsdev *ts;
    /*LV element*/
    lv_disp_buf_t disp_buf;
    lv_color_t color_buf[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    lv_disp_drv_t disp_drv;
    antfx_idev_t pointer;
} engine_frame_t;

/*
    A engine configuration is used by the system
    to load the user config file
*/
 typedef struct{
    uint16_t default_w;
    uint16_t default_h;
    uint8_t defaut_bbp;
    char* dev;
    char* tdev;
 } engine_config_t;
 
void display_init(engine_frame_t*, engine_config_t);
#ifdef USE_SDL2
void display_update(engine_frame_t* frame);
#endif
void display_release(engine_frame_t*);

void get_pointer_input(antfx_event_t* evt);
#endif