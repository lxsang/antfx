#include <signal.h>
#include "widgets/widgets.h"

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
    win.at = _P(50,50);
    win.size = _P(300,200);

    sty.bg_color = WHITE;
    sty.border_color = (pixel_t){33,34,32};
    sty.title_height = 15;
    sty.border=1;
    sty.title_color = (pixel_t){33,34,32};
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
        render();
        nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
        //nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        //px.x += 1;
        //px.y += 1;
    }
}
