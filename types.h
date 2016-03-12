#ifndef ANTFX_TYPE_H
#define ANTFX_TYPE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __unix__
#include <linux/time.h>
#else
#include <time.h>
#endif
#include <string.h>
#define COLOR8888   4
#define COLOR565    2
#define LOG printf
/*
    A engine frame is actually a frame buffer
    this struct will define  necessary info.
    to manipulate a frame
*/
typedef struct{
    short width;
    short height;
    uint8_t bbp;
    short xoffset;
    short yoffset;
    short line_length;
    uint8_t* buffer;
    int handle;
    int size;
} engine_frame_t;

/*
    A engine configuration is used by the system
    to load the user config file
*/
 typedef struct{
    char* name;
    short default_w;
    short default_h;
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
    signed short x;
    signed short y;
} point_t;
/*a custom shape*/
typedef struct __shape{
    point_t at;
    struct __shape* next;
}* shape;
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

/*a generic drawwing object*/
typedef struct __composite_shape{
    uint8_t type;
    void * shape;
    struct __composite_shape* childs;
} composite_shape_t;

typedef struct{

} font_t;


typedef struct{
    int value;
    uint8_t size;
} color_code_t;

#endif
