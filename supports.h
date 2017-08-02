#ifndef AFX_SUPPORT_H
#define AFX_SUPPORT_H

#include "types.h"
#include "engine.h"
#define COLOR(x) (_color_code(x,_screen.bbp))
#define draw_point(p,c) (_draw_point(p,COLOR(c),(point_t){0,0})
#define draw_rect(p) (_draw_rect(p,(point_t){0,0}))
#define draw_line(p,c) (_draw_line(p,COLOR(c),(point_t){0,0}))
#define draw_circle(c) (_draw_circle(c,(point_t){0,0}))
#define clear(c) (_clear(COLOR(c)))

#ifdef USE_BUFFER

extern uint8_t* _buffer;

#define all_black() (memset(_buffer, 0,_screen.size))
#define all_white() (memset(_buffer, 255,_screen.size))

#else

#define all_black() (memset(_screen.buffer, 0,_screen.size))
#define all_white() (memset(_screen.buffer, 255,_screen.size))

#endif

#define ORIGIN  ((point_t){0,0})

extern engine_frame_t _screen;
/*
    get the absolute the buffer pointer to the given position
*/
uint8_t* screen_position(point_t);
color_code_t _color_code(pixel_t px,uint8_t bbp);
void _draw_point(point_t,color_code_t,point_t);
void _put_pixel(point_t,color_code_t);
void _draw_rect(rect_t rec,point_t);
void _draw_line(line_t,color_code_t, point_t);
void _draw_circle(circle_t,point_t);
point_t _T(point_t a,point_t b);
void _clear(color_code_t);
void antfx_init(engine_config_t);
void antfx_release();
void render();
#endif