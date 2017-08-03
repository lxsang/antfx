#ifndef AFX_WINDOW
#define AFX_WINDOW

#include "../supports.h"

typedef struct{
    point_t size;
    uint8_t maximize;
    point_t at;
    composite_t children;
} afx_window_t;


typedef struct{
    uint8_t border;
    pixel_t bg_color;
    pixel_t border_color;
    pixel_t title_color;
    uint8_t title_height;
} afx_window_style_t;

void _draw_window(afx_window_t,afx_window_style_t,point_t);

#endif