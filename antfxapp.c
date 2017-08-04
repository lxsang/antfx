#include <signal.h>
#include "antfx.h"

void shutdown(int sig)
{
    antfx_release();
    exit(0);
}
int main(int argc, char* argv[]) 
{
    signal(SIGPIPE, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGINT, shutdown);
    engine_config_t conf;
    conf.default_w = 640;
    conf.default_h = 480;
    conf.defaut_bbp = 16;
    conf.name = "sdl2_engine.o";
    conf.dev = "/dev/fb1";
    // start display engine
    antfx_init(conf);
 
    afx_window_t win;
    afx_window_style_t sty;
    win.at = _P(50,10);
    win.size = _P(300,280);
    win.title = "Terminal";
    point_t px = ORIGIN;
    sty.bg_color = WHITE;
    sty.border_color = (pixel_t){187,185,187};
    sty.title_height = 20;
    sty.border=1;
    sty.title_bcolor = (pixel_t){222,220,222};
    sty.title_color = (pixel_t){100,100,100};
   
    afx_bitmap_t bmp;
    read_bitmap_file("test/test.bmp",&bmp);
    // try to read a bitmap font
    afx_font_t font;
    if(!load_font("/Users/mrsang/Documents/ushare/cwp/antfx/build/fonts/FreeMono12pt7b.bf",&font))
        LOG("Cannot load font\n");
    else 
        LOG("Font loaded \n");
    //shapes.at = _P(50,50);
    //clear(((pixel_t){0,0,255,0}));
    all_white();
    while(1)
    { 
        //draw_circle(c);
        //draw_rect(r);
        //draw_line(l);
        //draw_polygon(p);
        all_white();
        _draw_window(win,sty,ORIGIN);
        _draw_bitmap(bmp,_P(80,40));
        //_put_text("this is a text! 1234",_P(10,300),(pixel_t){0,122,204,0},SYS_FONT);
        render();
        nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
        //exit(0);
        //nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        px.x += 1;
        px.y += 1;
    }
}
