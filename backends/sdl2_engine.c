#include "../backend.h"
#include <SDL2/SDL.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
static int ds_pipe_fd[2];
static int pt_pipe_fd[2];
static int kb_pipe_fd[2];
static int scr_w;
static int scr_h;
static int scr_bbp;
static pid_t pid;

void display_init(engine_frame_t *frame, engine_config_t conf)
{
    frame->size = conf.default_h * conf.default_w * (conf.defaut_bbp / 8);
    frame->width = conf.default_w;
    frame->height = conf.default_h;
    frame->bbp = conf.defaut_bbp;
    frame->xoffset = 0;
    frame->yoffset = 0;
    frame->line_length = conf.default_w * (conf.defaut_bbp / 8);
    frame->buffer = (uint8_t *)malloc(frame->size);
    ;
    memset(frame->buffer, 0, frame->size);
    scr_w = frame->width;
    scr_h = frame->height;
    scr_bbp = frame->bbp;
    UNUSED(pipe(ds_pipe_fd));
    UNUSED(pipe(pt_pipe_fd));
    UNUSED(pipe(kb_pipe_fd));
    pid = fork();
    if (pid == 0)
    {
        // Child
        //dup2(outpipefd[0], STDIN_FILENO);
        //dup2(inpipefd[1], STDOUT_FILENO);
        //dup2(inpipefd[1], STDERR_FILENO);
        int buff_size = scr_w * scr_h * (scr_bbp / 8);
        int line_length = scr_w * (scr_bbp / 8);
        LOG("Buffsize is %d", buff_size);
        uint8_t *buff = (uint8_t *)malloc(buff_size);
        fd_set fd_in;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500;
        // create window
        static uint8_t __on = 0;
        static SDL_Window *window = NULL;
        static SDL_Renderer *renderer = NULL;
        SDL_Texture *texture = NULL;
        SDL_Event event;
        antfx_event_t pipe_data_out;
        pipe_data_out.data[0] = 0;
        pipe_data_out.data[1] = 0;
        SDL_Init(SDL_INIT_VIDEO);
        __on = 1;
        window = SDL_CreateWindow("SDL2 engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scr_w, scr_h, SDL_WINDOW_SHOWN);
        if (!window)
        {
            ERROR("Error could not create window: %s\n", SDL_GetError());
            return;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer)
        {
            ERROR("Error could not create renderer: %s\n", SDL_GetError());
            return;
        }
        if (scr_bbp == 32)
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, scr_w, scr_h);
        else
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, scr_w, scr_h);

        if (!texture)
        {
            ERROR("Error could not create texture: %s\n", SDL_GetError());
            return;
        }
        LOG("engine init successful\n");
        while (1)
        {
            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT)
            {
                LOG("Quit event");
                break;
            }
            else
            {
                switch (event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    pipe_data_out.type = AFX_EVT_DOWN;
                    pipe_data_out.data[0] = (uint16_t)event.motion.x;
                    pipe_data_out.data[1] = (uint16_t)event.motion.y;
                    write(pt_pipe_fd[1], (uint8_t *)(&pipe_data_out), sizeof(pipe_data_out));
                    break;

                case SDL_MOUSEBUTTONUP:
                    pipe_data_out.type = AFX_EVT_RELEASE;
                    pipe_data_out.data[0] = (uint16_t)event.motion.x;
                    pipe_data_out.data[1] = (uint16_t)event.motion.y;
                    write(pt_pipe_fd[1], (uint8_t *)(&pipe_data_out), sizeof(pipe_data_out));
                    break;
                case SDL_MOUSEMOTION:
                    if (abs(event.motion.x - pipe_data_out.data[0]) > 5 || abs(event.motion.y - pipe_data_out.data[1]) > 5)
                    {
                        pipe_data_out.type = AFX_EVT_MOVE;
                        pipe_data_out.data[0] = (uint16_t)event.motion.x;
                        pipe_data_out.data[1] = (uint16_t)event.motion.y;
                        write(pt_pipe_fd[1], (uint8_t *)(&pipe_data_out), sizeof(pipe_data_out));
                    }
                    break;

                default:
                    break;
                }
            }

            FD_ZERO(&fd_in);
            FD_SET(ds_pipe_fd[0], &fd_in);
            int rc = select(ds_pipe_fd[0] + 1, &fd_in, NULL, NULL, &timeout);
            if (FD_ISSET(ds_pipe_fd[0], &fd_in))
            {
                int recv = 0;
                while ((rc = read(ds_pipe_fd[0], buff + recv, buff_size - recv)) > 0)
                {
                    recv += rc;
                }
            }
            int stat;
            stat = SDL_UpdateTexture(texture, NULL, buff, line_length);
            if (stat == -1)
            {
                LOG("Could not update texture: %s\n", SDL_GetError());
                break;
            }
            stat = SDL_RenderClear(renderer);
            if (stat == -1)
            {
                LOG("Could not clear renderer: %s\n", SDL_GetError());
                break;
            }
            stat = SDL_RenderCopy(renderer, texture, NULL, NULL);
            if (stat == -1)
            {
                LOG("Could not copy texture to renderer: %s\n", SDL_GetError());
                break;
            }
            SDL_RenderPresent(renderer);
            // wait for 30 mmilisecond .-i.e 30ffs
            //sleep(1);
            nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
        }
        if (texture)
            SDL_DestroyTexture(texture);
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
        if (__on)
            SDL_Quit();
        __on = 0;
        if (buff)
            free(buff);
        _exit(1);
    }
    // parent here
    close(ds_pipe_fd[0]);
    close(pt_pipe_fd[1]);
    close(kb_pipe_fd[1]);
}

void display_update(engine_frame_t *frame)
{
    UNUSED(write(ds_pipe_fd[1], frame->buffer, frame->size));
}

void display_release(engine_frame_t *frame)
{

    if (pid > 0)
        kill(pid, SIGKILL);
    if (frame->buffer)
        free(frame->buffer);
    frame->buffer = NULL;
    LOG("Engine release successful");
}

void get_pointer_input(engine_frame_t *screen)
{
    fd_set fd_in;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;
    int size = sizeof(screen->pointer.evt);
    uint8_t *ptr = (uint8_t *)(&screen->pointer.evt);
    FD_ZERO(&fd_in);
    FD_SET(pt_pipe_fd[0], &fd_in);
    int rc = select(pt_pipe_fd[0] + 1, &fd_in, NULL, NULL, &timeout);
    if (FD_ISSET(pt_pipe_fd[0], &fd_in))
    {
        int recv = 0;
        while (recv != size && (rc = read(pt_pipe_fd[0], ptr + recv, size - recv)) > 0)
        {
            recv += rc;
        }
        //printf("Event %d x:%d y:%d\n", pdata->type, pdata->data[0], pdata->data[1]);
    }
}