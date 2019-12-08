#ifndef AFX_SUPPORT_H
#define AFX_SUPPORT_H

#include "types.h"
#include "backend.h"

#define S_COMPOSITE 1
#define S_LINE      2
#define S_RECT      3
#define S_CIRCLE    4
#define S_POLY      5
#define ORIGIN  ((point_t){0,0})
#define COLOR(x) (_color_code(x,_screen.bbp))
#define _P(x,y) ((point_t){x,y})
#define draw_point(p,c) (_draw_point(p,COLOR(c),ORIGIN)
#define draw_rect(p) (_draw_rect(p,ORIGIN))
#define draw_line(l) (_draw_line(l,ORIGIN))
#define draw_circle(c) (_draw_circle(c,ORIGIN))
#define draw_polygon(p) (_draw_polygon(p,ORIGIN))
#define draw_composite(c) (_draw_composite(c,ORIGIN))
#define clear(c) (_clear(COLOR(c)))

#define composite_shape() ( (composite_t){ ORIGIN, list_init()})
#define add_shape(l,i) (list_put(&l.shapes,i))
#define last_shape(l) (list_last(l.shapes))
#define remove_shape(l,i) (list_remove(l.shapes,i))
#define composite_size(l) (list_size(l.shapes))
#define shape_at(l,i) (list_at(l.shapes,i))
#define composite_empty(l) (list_empty(l.shapes))
#define new_shape(t,v) (new_list_item(t, (void*) v))
#define destroy_composite(l) (list_free(&l.shapes))

#ifdef USE_BUFFER
#define BUFFER _screen.swap_buffer
#else
#define BUFFER _screen.buffer
#endif

#define all_black() (memset(BUFFER, 0,_screen.size))
#define all_white() (memset(BUFFER, 255,_screen.size))



extern engine_frame_t _screen;
/*
    get the absolute the buffer pointer to the given position
*/
uint8_t* screen_position(point_t);
color_code_t _color_code(color_t px,uint8_t bbp);
void _draw_point(point_t,color_code_t,point_t);
void _put_pixel(point_t,color_code_t);
void _draw_rect(rect_t rec,point_t);
void _draw_line(line_t, point_t);
void _draw_circle(circle_t,point_t);
point_t _T(point_t a,point_t b);
void _clear(color_code_t);
void _draw_polygon(polygon_t,point_t);
void _draw_composite(composite_t,point_t);

#endif