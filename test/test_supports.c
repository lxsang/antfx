#include "../supports.h"
#include <signal.h>
#include "../engine.h"

composite_t shapes;
void shutdown(int sig)
{
    antfx_release();
    destroy_composite(shapes);
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
 
    // draw some thing on the screen
    point_t px = ORIGIN; 
    
    circle_t c;
    c.at = (point_t){100,100};
    c.color = (pixel_t){255,0,0,0};
    c.bcolor = (pixel_t){100,100,255,0};
    c.stroke = 2;
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
    l.from = (point_t){300,300};
    l.to = (point_t){200,200};
    l.color = (pixel_t){0,0,255,0};
    l.stroke = 3;

    polygon_t p;
    p.stroke = 2;
    p.color =  (pixel_t){255,0,255,0};
    p.connected = 1;
    p.size = 3;
    point_t _p_[3];
    _p_[0] = _P(200,10);
    _p_[1] = _P(300,50);
    _p_[2] = _P(250,100);
    p.points = _p_;

    shapes = composite_shape();
    add_shape(shapes,  new_shape( S_LINE, &l) );
    add_shape(shapes,  new_shape( S_CIRCLE, &c) );
    add_shape(shapes,  new_shape( S_RECT, &r) );
    add_shape(shapes, new_shape(S_POLY, & p));

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
        shapes.at = px;
        draw_composite(shapes);
        render();
        nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
        //nanosleep((const struct timespec[]){{0, 30000000L}}, NULL);
        px.x += 1;
        px.y += 1;
    }
}
