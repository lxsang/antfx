#include "engine_interface.h"

engine_frame_t screen;
void shutdown(int sig)
{
    if(screen.buffer)
        engine_release(&screen);
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
    // start display engine
    engine_init(&screen,conf);

    // draw some thing on the screen
    int x = 100; int y = 100;       // Where we are going to put the pixel
    int location;
    printf("drawing some thing\n");

    int i = 0;

    for(i;i< screen.height*screen.width*(screen.bbp/8);i++)
        printf("%d ", screen.buffer[i]);

    printf("\n\n");
    // Figure out where in memory to put the pixel
    for (y = 100; y < 300; y++)
        for (x = 100; x < 300; x++) {

            location = (x+screen.xoffset) * (screen.bbp/8) +
                       (y+screen.yoffset) * screen.line_length;

            if (screen.bbp == 32) {
                *(screen.buffer + location) = 100;        // Some blue
                *(screen.buffer + location + 1) = 15+(x-100)/2;     // A little green
                *(screen.buffer + location + 2) = 200-(y-100)/5;    // A lot of red
                *(screen.buffer + location + 3) = 0;      // No transparency
        //location += 4;
            } else  { //assume 16bpp
                int b = 10;
                int g = (x-100)/6;     // A little green
                int r = 31-(y-100)/16;    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(screen.buffer + location)) = t;
            }

        }
    while(1){};
}