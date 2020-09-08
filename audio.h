#ifndef AUDIO_H
#define AUDIO_H
#include <pulse/pulseaudio.h>

#include "bst.h"
#include "default.h"

typedef enum
{
    A_EVENT_PENDING,
    A_EVENT_OK,
    A_EVENT_FAIL

} antfx_audio_write_event_t;

typedef struct
{
    char name[ANTFX_MAX_STR_BUFF_SZ];
    char desc[ANTFX_MAX_STR_BUFF_SZ];
    pa_channel_map map;
    pa_cvolume volume;
    uint8_t card;
    int index;
} antfx_audio_dev_t;

typedef struct
{
    int count;
    bst_node_t *devices;
} antfx_audio_dev_list_t;

typedef struct
{
    void *mem;
    size_t len;
    uint8_t ready;
    uint8_t writable;
} antfx_audio_buffer_t;

typedef struct
{
    pa_context *context;
    pa_mainloop_api *mainloop_api;
    pa_mainloop *mainloop;
    pa_io_event *io_event;
    pa_stream *input_stream;
    pa_stream *output_stream;
    antfx_audio_buffer_t buffer;
    pthread_mutex_t lock;
} antfx_audio_session_t;

typedef struct
{
    antfx_audio_dev_list_t inputs;
    antfx_audio_dev_list_t outputs;
    antfx_audio_session_t session;
} antfx_audio_t;

int antfx_audio_init();
void antfx_audio_release();
void antfx_audio_dump();
int antfx_audio_writable();
int antfx_audio_writable_size();
void antfx_audio_flush();
int antfx_audio_write(void *, int);
int antfx_audio_set_input(int, const char *, int (*callback)(void *, int));
int antfx_audio_write_event_fd(int fd, int (*callback)(int, unsigned char **, int, antfx_audio_write_event_t));
void antfx_audio_remove_input();
int antfx_audio_set_output(int, const char *);

void antfx_audio_output_pause();
void antfx_audio_output_resume();
int antfx_audio_output_is_paused();

int antfx_audio_set_input_volume(int);
int antfx_audio_set_output_volume(int);

#endif