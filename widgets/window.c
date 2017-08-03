#include "window.h"

void _draw_window(afx_window_t conf,afx_window_style_t sty,point_t tr)
{
    point_t org = _T(conf.at,tr);
    rect_t win;
    rect_t title;

    win.at = ORIGIN;
    win.of = conf.size;
    win.stroke = sty.border;
    win.color = sty.border_color;
    win.bcolor = sty.bg_color;
    win.fill = 1;
    
    title.at = _P(sty.border,sty.border);
    title.of = _P(conf.size.x-2*sty.border,sty.title_height);
    title.fill = 1;
    title.stroke = 0;
    title.bcolor = sty.title_color;

    _draw_rect(win,org);
    _draw_rect(title,org);

    rect_t close;
    close.at = _P(sty.border + 5,sty.border+1);
    close.of = _P(8,8);
    close.fill = 1;
    close.stroke = 0;
    close.bcolor = (pixel_t){252,99,93};
    _draw_rect(close, org);

    close.at = _P(sty.border + 17,sty.border+1);
    close.bcolor = (pixel_t){254,192,65};
    _draw_rect(close, org);

    close.at = _P(sty.border + 29,sty.border+1);
    close.bcolor = (pixel_t){54,206,76};
    _draw_rect(close, org);
    // draw all children here
}