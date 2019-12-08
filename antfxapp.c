#include <signal.h>
#ifdef __unix__
#include <linux/time.h>
#else
#include <time.h>
#endif

#include "antfx.h"

void shutdown(int sig)
{
    antfx_release();
    exit(0);
}
int main(int argc, char* argv[]) 
{
    char* font_name = NULL;
    if(argc == 2)
    {
        font_name = argv[1];
        printf("Font name %s \n", font_name);
    }
    signal(SIGPIPE, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGINT, shutdown);
    engine_config_t conf;
    conf.default_w = 480;
    conf.default_h = 320;
    conf.defaut_bbp = 16;
    conf.name = "sdl2_engine.o";
    conf.dev = "/dev/fb1";
    // start display engine
    antfx_init(conf);

    afx_font_t font;
    if(font_name == NULL || !load_font(font_name,&font))
    {
        LOG("Cannot load font. Use system font\n");
        font = SYS_FONT;
    }
    else 
        LOG("Font loaded \n");
 
    afx_window_t win;
    afx_window_style_t sty;
    win.at = _P(50,10);
    win.size = _P(300,280);
    win.title = "Terminal";
    point_t px = ORIGIN;
    sty.bg_color = WHITE;
    sty.border_color = (color_t){187,185,187,0};
    sty.title_height = 20;
    sty.border=1;
    sty.title_bcolor = (color_t){222,220,222,0};
    sty.title_color = (color_t){100,100,100,0};
    sty.font = font;
   
    afx_bitmap_t bmp;
    read_bitmap_file("test/test.bmp",&bmp);
    // try to read a bitmap font
    //shapes.at = _P(50,50);
    //clear(((color_t){0,0,255,0}));
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
        _put_text("this is a text! 1234\n new line are not implemented",_P(10,290),(color_t){0,122,204,0},font);
        render();
        nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        px.x += 1;
        px.y += 1;
    }
}
