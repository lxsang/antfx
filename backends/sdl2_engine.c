#include "../backend.h"
#include <SDL2/SDL.h>
#include <sys/types.h>
#include <unistd.h>

engine_frame_t* __screen;
static uint8_t __on = 0;
static SDL_Window * window = NULL;
static SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL; 

void display_update()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    pid_t pid;
    switch (event.type)
    {
        case SDL_QUIT:
            pid = getpid();
            kill(pid, SIGINT);
            break;
    }
    int stat;
    stat = SDL_UpdateTexture(texture, NULL, __screen->buffer, __screen->line_length);
    if(stat == -1)
    {
        LOG("Could not update texture: %s\n",SDL_GetError());
        return;
    }
    stat = SDL_RenderClear(renderer);
    if(stat == -1)
    {
        LOG("Could not clear renderer: %s\n",SDL_GetError());
        return;
    }
    stat = SDL_RenderCopy(renderer, texture, NULL, NULL);
    if(stat == -1)
    {
        LOG("Could not copy texture to renderer: %s\n",SDL_GetError());
        return;
    }
    SDL_RenderPresent(renderer);
    // wait for 30 mmilisecond .-i.e 30ffs
    //sleep(1);
    //nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
}
void display_init(engine_frame_t* frame, engine_config_t conf)
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
    // create window
    //SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);
    __on = 1;
    window = SDL_CreateWindow("SDL2 engine",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, __screen->width, __screen->height, SDL_WINDOW_SHOWN);
    if(!window) {
        LOG("Error could not create window: %s\n",SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
    {
        LOG("Error could not create renderer: %s\n",SDL_GetError());
        exit(1);
    }
    if(__screen->bbp == 32)
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, __screen->width, __screen->height);
    else
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, __screen->width, __screen->height);
    
    if(!texture)
    {
        LOG("Error could not create texture: %s\n",SDL_GetError());
        exit(1);
    }
    LOG("engine init successful\n");
}

void display_release(engine_frame_t* frame)
{
    if(texture)
        SDL_DestroyTexture(texture);
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);
    if(__on)
        SDL_Quit();
    __on = 0;
    if(frame->buffer) free(frame->buffer);
    frame->buffer = NULL;
    LOG("Engine release successful\n");
}