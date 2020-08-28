
#include <stdio.h>
#include <string.h>
#include <pulse/error.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "conf.h"
#include "media.h"

#define SET_STATUS(s)                 \
    pthread_mutex_lock(&g_mctl.lock); \
    g_mctl.status = s;                \
    pthread_mutex_unlock(&g_mctl.lock);

#define SET_MODE(s)                 \
    pthread_mutex_lock(&g_mctl.lock); \
    g_mctl.mode = s;                \
    pthread_mutex_unlock(&g_mctl.lock);


static antfx_music_ctl_t g_mctl = {
    MUSIC_STOP,
    MUSIC_ONCE,
    0,
    NULL,
    NULL,
    NULL,
    0,
    0,
    {},
    0,
    {0}};


static void antfx_music_release_ctrl()
{
    LOG("Clean up music controller");
    int error;
    if (g_mctl.simple != NULL)
    {
        if (pa_simple_drain(g_mctl.simple, &error) != 0)
        {
            ERROR("pa_simple_drain: %s\n", pa_strerror(error));
        }
        pa_simple_free(g_mctl.simple);
        g_mctl.simple = NULL;
    }
    if (g_mctl.mh != NULL)
    {
        if (mpg123_close(g_mctl.mh) != MPG123_OK)
        {
            ERROR("Unable to close handle");
        }
        mpg123_delete(g_mctl.mh);
        g_mctl.mh = NULL;
    }
    // if (g_mctl.buffer)
    //   free(g_mctl.buffer);
    g_mctl.buffer = NULL;
}

static void *antfx_music_ctl_thread(void *user)
{
    size_t done;
    char *buffer;
    int error = 0;
    LOG("Start the music control with status: %d", g_mctl.status);
    while (g_mctl.status != MUSIC_STOP)
    {
        switch (g_mctl.status)
        {
        case MUSIC_NEW:
            antfx_music_release_ctrl();
            // create new ctl
            g_mctl.mh = mpg123_new(NULL, &error);
            if (error != MPG123_OK)
            {
                ERROR("Unable to initialise handle: %s", mpg123_strerror(g_mctl.mh));
                SET_STATUS(MUSIC_STOP);
            }
            else
            {
                error = mpg123_open(g_mctl.mh, g_mctl.current_song);
                if (error != MPG123_OK)
                {
                    ERROR("Unable to open file %s: %s", g_mctl.current_song, mpg123_strerror(g_mctl.mh));
                    SET_STATUS(MUSIC_STOP);
                }
                else
                {
                    error = mpg123_getformat(g_mctl.mh, (long *)&g_mctl.sample_spec.rate, (int *)&g_mctl.sample_spec.channels, &g_mctl.encoding);
                    if (error != MPG123_OK)
                    {
                        ERROR("Unable to fetch media format: %s", mpg123_strerror(g_mctl.mh));
                        SET_STATUS(MUSIC_STOP);
                    }
                    else
                    {
                        g_mctl.total_frames = mpg123_framelength(g_mctl.mh);
                        g_mctl.sample_spec.format = PA_SAMPLE_S16LE;
                        g_mctl.simple = pa_simple_new(
                            NULL,
                            "antfx",
                            PA_STREAM_PLAYBACK,
                            NULL,
                            "music",
                            &g_mctl.sample_spec,
                            NULL,
                            NULL,
                            &error);

                        if (g_mctl.simple == NULL)
                        {
                            ERROR("pa_simple_new: %s\n", pa_strerror(error));
                            SET_STATUS(MUSIC_STOP);
                        }
                        else
                        {
                            SET_STATUS(MUSIC_PLAYING);
                        }
                    }
                }
            }
            break;

        case MUSIC_PLAYING:
            error = mpg123_decode_frame(g_mctl.mh, &g_mctl.current_frame, &g_mctl.buffer, &done);
            if (error == MPG123_OK)
            {
                //printf("frame %ld/%ld\n",g_mctl.current_frame, g_mctl.total_frames );
                if (pa_simple_write(g_mctl.simple, g_mctl.buffer, (size_t)done, &error) != 0)
                {
                    ERROR("pa_simple_write: %s\n", pa_strerror(error));
                    SET_STATUS(MUSIC_STOP);
                }
            }
            else
            {
                ERROR("Unable to decode frame: %s", mpg123_strerror(g_mctl.mh));
                SET_STATUS(MUSIC_STOP);
            }
            break;

        case MUSIC_PAUSE:
            usleep(100000); // 100 ms
            break;

        default:
            break;
        }
    }
    antfx_music_release_ctrl();
    LOG("Stop the playback thread");
    return 0;
}

const antfx_music_ctl_t* antfx_music_get_ctrl()
{
    return &g_mctl;
}

int antfx_music_play(const char *song)
{
    strncpy(g_mctl.current_song, song, MAX_CONF_SIZE);
    LOG("Playing %s", g_mctl.current_song);
    if (g_mctl.status == MUSIC_STOP)
    {
        SET_STATUS(MUSIC_NEW);
        if (pthread_create(&g_mctl.tid, NULL, antfx_music_ctl_thread, NULL) != 0)
        {

            ERROR("Error creating weather thread: %s", strerror(errno));
            SET_STATUS(MUSIC_STOP);
            return -1;
        }

        if (pthread_detach(g_mctl.tid) != 0)
        {
            ERROR("Unable to detach weather thread: %s", strerror(errno));
            SET_STATUS(MUSIC_STOP);
            return -1;
        }
    }
    else
    {
        SET_STATUS(MUSIC_NEW);
    }
    return 0;
}
int antfx_music_pause()
{
    SET_STATUS(MUSIC_PAUSE);
}
int antfx_music_resume()
{
    SET_STATUS(MUSIC_PLAYING);
}
int antfx_music_stop()
{
    SET_STATUS(MUSIC_STOP);
    memset(g_mctl.current_song, 0, MAX_CONF_SIZE);
}

void anfx_music_init()
{
    pthread_mutex_init(&g_mctl.lock, NULL);
    mpg123_init();
}
void anfx_music_release()
{
    pthread_mutex_destroy(&g_mctl.lock);
    mpg123_exit();
}

void antfx_music_set_mode(antfx_music_play_mode_t m)
{
    SET_MODE(m);
}