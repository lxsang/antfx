#ifndef MEDIA_H
#define MEDIA_H
#include <mpg123.h>
#include <pulse/simple.h>
#include "default.h"
#include "bst.h"

#include "audio.h"

typedef enum
{
    M_MUSIC_MODE,
    M_FM_MODE,
    M_NONE
} antfx_media_playback_mode_t;

typedef enum
{
    MUSIC_RND,
    MUSIC_ONCE,
    MUSIC_1B1
} antfx_media_music_play_mode_t;

typedef enum
{
    MUSIC_PLAYING,
    MUSIC_STOP,
    MUSIC_PAUSE,
    MUSIC_SKIP
} antfx_media_music_status_t;

typedef struct
{
    antfx_media_music_status_t status;
    antfx_media_music_play_mode_t mode;
    mpg123_handle* mh;
    unsigned char *buffer;
    int buffer_len;
    off_t total_frames;
    off_t current_frame;
    int encoding;
    char current_song[ANTFX_MAX_STR_BUFF_SZ];
    bst_node_t* songs;
    int total_songs;
    int current_page;
} antfx_media_music_ctl_t;

typedef struct {
    antfx_media_music_ctl_t music;
    antfx_media_playback_mode_t mode;
} antfx_media_ctl_t;


int antfx_media_music_play(const char* song);
void antfx_media_music_pause();
void antfx_media_music_resume();
void antfx_media_music_stop();
void antfx_media_music_play_next();
void antfx_media_init();
void antfx_media_release();

int antfx_media_fm_start(float, int );
void antfx_media_fm_stop();

#endif