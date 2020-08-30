#ifndef MEDIA_H
#define MEDIA_H
#include <mpg123.h>
#include <pulse/simple.h>
#include "default.h"
#include "bst.h"
typedef enum
{
    MUSIC_RND,
    MUSIC_ONCE,
    MUSIC_1B1
} antfx_music_play_mode_t;

typedef enum{
    A_MUSIC_MODE,
    A_FM_MODE,
    A_NONE
} antfx_audio_mode_t;

typedef enum
{
    MUSIC_NEW,
    MUSIC_PLAYING,
    MUSIC_STOP,
    MUSIC_PAUSE,
    MUSIC_SKIP
} antfx_music_status_t;

typedef struct
{
    antfx_music_status_t status;
    antfx_music_play_mode_t mode;
    pthread_t tid;
    mpg123_handle *mh;
    pa_simple *simple;
    unsigned char *buffer;
    off_t total_frames;
    off_t current_frame;
    pa_sample_spec sample_spec;
    int encoding;
    pthread_mutex_t lock;
    char current_song[ANTFX_MAX_STR_BUFF_SZ];
} antfx_music_ctl_t;


typedef struct {
    char startup_sound[ANTFX_MAX_STR_BUFF_SZ];
    bst_node_t* inputs;
    bst_node_t* outputs;
    antfx_audio_mode_t mode;
    int n_in;
    int n_out;
} antfx_audio_t;

int antfx_music_play(const char* song);
int antfx_music_pause();
int antfx_music_resume();
int antfx_music_stop();
int antfx_music_init();
void antfx_music_set_mode(antfx_music_play_mode_t m);

void anfx_music_init();
void anfx_music_release();

const antfx_music_ctl_t* antfx_music_get_ctrl();
#endif