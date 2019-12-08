#ifndef ANTFX_TYPE_H
#define ANTFX_TYPE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "list.h"
#define COLOR8888   4
#define COLOR565    2
#define WHITE (pixel_t){255,255,255,0}
#define BLACK (pixel_t){0,0,0,0}
#define LOG printf
/*
    A engine frame is actually a frame buffer
    this struct will define  necessary info.
    to manipulate a frame
*/
typedef struct{
    uint32_t value;
    uint8_t size;
} color_code_t;

typedef struct{
    uint16_t width;
    uint16_t height;
    uint8_t bbp;
    int16_t xoffset;
    int16_t yoffset;
    uint16_t line_length;
    uint8_t* buffer;
#ifdef USE_BUFFER
    uint8_t* swap_buffer;
#endif
    int handle;
    uint32_t size;
} engine_frame_t;

/*
    A engine configuration is used by the system
    to load the user config file
*/
 typedef struct{
    char* name;
    uint16_t default_w;
    uint16_t default_h;
    uint8_t defaut_bbp;
    char* dev;
 } engine_config_t;

 /*
    a pixel data
 */
typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} pixel_t;

typedef struct{
    int16_t x;
    int16_t y;
} point_t;

/* a rectangle*/
typedef struct{
    point_t at;
    point_t of;
    uint8_t fill;
    pixel_t color;
    pixel_t bcolor;
    uint8_t stroke;
} rect_t;

typedef struct{
    point_t from;
    point_t to;
    uint8_t stroke;
    pixel_t color;
} line_t;
/*a circle*/
typedef struct{
    point_t at;
    short r;
    uint8_t fill;
    pixel_t color;
    pixel_t bcolor;
    uint8_t stroke;
} circle_t;

typedef  list shapes_t;
typedef  list shape_t;

typedef struct{
    point_t at;
    shapes_t shapes;
} composite_t;

typedef struct{
    pixel_t color;
    uint8_t stroke;
    uint8_t fill;
    point_t* points;
    uint32_t size;
    uint8_t connected;
} polygon_t;

typedef struct{

} font_t;


#endif
