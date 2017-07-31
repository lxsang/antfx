#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
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

#endif