#include "../engine.h"
#include <SDL2/SDL.h>
#include <pthread.h>

engine_frame_t* __screen;
static uint8_t __on = 1;
void _simulate_frame_buffer()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2 engine",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, __screen->width, __screen->height, 0);

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture;
    if(__screen->bbp == 32)
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, __screen->width, __screen->height);
    else
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, __screen->width, __screen->height);
    
    while (__on)
    {
        //printf("render with %d\n",__screen->line_length);
        SDL_UpdateTexture(texture, NULL, __screen->buffer, __screen->line_length);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        // wait for 30 mmilisecond .-i.e 30ffs
        nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
    }
    __on = 2;
    printf("Engine release successful\n");
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void engine_init(engine_frame_t* frame, engine_config_t conf)
{
    frame->size = conf.default_h*conf.default_w*(conf.defaut_bbp/8);
    frame->width = conf.default_w;
    frame->height = conf.default_h;
    frame->bbp = conf.defaut_bbp;
    frame->xoffset = 0;
    frame->yoffset = 0;
    frame->line_length = conf.default_w*(conf.defaut_bbp/8);
    frame->buffer = (uint8_t*) malloc(frame->size);;
    memset(frame->buffer, 0,frame->size);
    __screen = frame;
    __on = 1;
    // create new thread to draw the buffer
    pthread_t newthread;
    if (pthread_create(&newthread , NULL,(void *(*)())_simulate_frame_buffer, NULL) != 0)
			perror("pthread_create");
		else
		{
			//reclaim the stack data when thread finish
			pthread_detach(newthread) ;
		}
    printf("engine init successful\n");

    
}

void engine_release(engine_frame_t* frame)
{
    __on = 0;
    while(__on == 0);
    if(__screen->buffer) free(__screen->buffer);
    frame->buffer = NULL;
}