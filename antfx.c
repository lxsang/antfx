#include "supports.h"
#include <signal.h>
#include "engine.h"

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
    conf.default_w = 480;
    conf.default_h = 320;
    conf.defaut_bbp = 16;
    conf.name = "sdl2_engine.o";
    conf.dev = "/dev/fb1";
    // start display engine
    antfx_init(conf);
 
    // draw some thing on the screen
    int x = 10; int y = 10;       // Where we are going to put the pixel
    int location;
    int i = 0;
    point_t px = ORIGIN; 
    
    circle_t c;
    c.at = (point_t){100,100};
    c.color = (pixel_t){255,0,0,0};
    c.bcolor = (pixel_t){100,255,100,0};
    c.stroke = 5;
    c.fill = 1;
    c.r = 50;

    rect_t r;
    r.at = (point_t){150,140};
    r.of = (point_t){50,50};
    r.bcolor = (pixel_t){100,255,100,0};
    r.color = (pixel_t){255,0,0,0};
    r.fill = 1;
    r.stroke = 3;

    line_t l;
    l.from = (point_t){10,10};
    l.to = (point_t){100,100};
    l.stroke = 2;
    //clear(((pixel_t){0,0,255,0}));
    all_white();
    while(1)
    {
        draw_circle(c);
        draw_rect(r);
        draw_line(l,((pixel_t){0,0,255,0}));
        render();
        nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
        //nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        //px.x += 10;
        //px.y += 5;
    }
}