#include "window.h"

void _draw_window(afx_window_t conf,afx_window_style_t sty,point_t tr)
{
    point_t org = _T(conf.at,tr);
    rect_t win;
    rect_t title;
    afx_font_t font = sty.font.loaded == 1 ? sty.font:SYS_FONT;
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
    title.bcolor = sty.title_bcolor;

    _draw_rect(win,org);
    _draw_rect(title,org);

    rect_t close;
    close.at = _P(sty.border + 2,sty.border+2);
    close.of = _P(7,7);
    close.fill = 1;
    close.stroke = 0;
    close.bcolor = (color_t){252,99,93,0};
    _draw_rect(close, org);

    close.at = _P(sty.border + 12,sty.border+2);
    close.bcolor = (color_t){254,192,65,0};
    _draw_rect(close, org);

    close.at = _P(sty.border + 22,sty.border+2);
    close.bcolor = (color_t){54,206,76,0};
    _draw_rect(close, org);

    if(conf.title)
    {
        uint16_t len  = strlen(conf.title);
        uint16_t offset = len*4; // width 4 font
        _put_text(conf.title,
            _T(org,_P(conf.size.x/2 - offset ,sty.border + sty.title_height/2 + font.yAdvance/4 -1 )),
            sty.title_color,font); 
    }
    // draw all children here
}