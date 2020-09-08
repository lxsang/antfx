
#include <stdio.h>
#include <string.h>
#include <pulse/error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include "log.h"
#include "media.h"
#include "hw.h"
#include "conf.h"
#include "utils.h"

#define SET_STATUS(s)                        \
    antfx_conf_t *conf = antfx_get_config(); \
    conf->media.music.status = s;

#define SET_MODE(s)                          \
    antfx_conf_t *conf = antfx_get_config(); \
    conf->media.music.mode = s;

static void antfx_media_music_release_ctrl(antfx_media_music_ctl_t *ctl)
{
    LOG("Clean up music controller");
    if (ctl->mh != NULL)
    {
        if (mpg123_close(ctl->mh) != MPG123_OK)
        {
            ERROR("Unable to close handle");
        }
        mpg123_delete(ctl->mh);
        ctl->mh = NULL;
    }
    ctl->buffer = NULL;
    ctl->buffer_len = 0;
    SET_STATUS(MUSIC_STOP);
}

/*
static void antfx_media_set_output(void)
{
    antfx_conf_t *conf = antfx_get_config();
    if(antfx_audio_set_input_volume(conf->fav.input_volume) == -1)
    {
        ERROR("Unable to set initial input volume value %d", conf->fav.input_volume);
    }
    if(antfx_audio_set_output_volume(conf->fav.output_volume) == -1)
    {
        ERROR("Unable to set initial output volume value %d", conf->fav.output_volume);
    }
    if (antfx_audio_set_output(conf->fav.output, "Default output") == -1)
    {
        ERROR("Unable to set default output");
    }
}
*/
static int antfx_media_fm_cb(void *data, int len)
{
    if (!antfx_audio_writable())
    {
        ERROR("audio out is not writable");
        return -1;
    }
    int size = antfx_audio_writable_size();
    if (size < len)
    {
        LOG("Buffer is currently full");
        return -1;
    }
    if (antfx_audio_write(data, len) == -1)
    {
        ERROR("Unable to write data to output");
        return -1;
    }
    return 0;
}
static void antfx_audio_music_load_playlist()
{
    DIR *d;
    antfx_conf_t *conf = antfx_get_config();
    struct dirent *dir;
    conf->media.music.total_songs = 0;
    if (conf->media.music.songs)
    {
        bst_free(conf->media.music.songs, 1);
        conf->media.music.songs = NULL;
    }

    d = opendir(conf->fav.music_path);
    if (d != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (regex_match("^.*\\.mp3$", dir->d_name, 0, NULL))
            {
                conf->media.music.songs = bst_insert(conf->media.music.songs, conf->media.music.total_songs, strdup(dir->d_name), 1);
                conf->media.music.total_songs++;
            }
        }
        closedir(d);
        LOG("Playlist loaded with %d entries", conf->media.music.total_songs);
    }
    else
    {
        ERROR("Unable to open music library directory: %s", conf->fav.music_path);
    }
}
void *antfx_media_audio_thread(void *data)
{
    antfx_audio_init();
}

static int antfx_media_music_playback(int fd, unsigned char **data, int max_len, antfx_audio_write_event_t e)
{
    antfx_conf_t *conf = antfx_get_config();
    int ws, error;
    size_t done;
    if (conf->media.music.mh == NULL || conf->audio.session.output_stream == NULL)
    {
        antfx_media_music_release_ctrl(&conf->media.music);
        if (fd > 0)
            close(fd);
        return -1;
    }
    if (e == A_EVENT_FAIL)
    {
        LOG("Event fail release controller");
        antfx_media_music_release_ctrl(&conf->media.music);
        if (fd > 0)
            close(fd);
        return -1;
    }
    if (e == A_EVENT_PENDING)
    {
        return 0;
    }
    switch (conf->media.music.status)
    {
    case MUSIC_PAUSE:
        return 0;
    case MUSIC_PLAYING:
        if (max_len == 0)
        {
            return 0;
        }
        if (conf->media.music.buffer_len == 0)
        {
            error = mpg123_decode_frame(conf->media.music.mh, &conf->media.music.current_frame, &conf->media.music.buffer, &done);
            if (error == MPG123_OK)
            {
                conf->media.music.buffer_len = done;
            }
            else
            {
                ERROR("Unable to decode frame: %s", mpg123_strerror(conf->media.music.mh));
                //antfx_media_music_release_ctrl(&conf->media.music);
                //if(fd > 0) close(fd);
                return -1;
            }
        }
        if (conf->media.music.buffer_len > 0 && conf->media.music.buffer_len < max_len)
        {
            *data = (unsigned char *)conf->media.music.buffer;
            ws = conf->media.music.buffer_len;
            conf->media.music.buffer_len = 0;
            return ws;
        }
        // drain the audio stream
        //if(conf->media.music.status == MUSIC_PAUSE)
        //    antfx_audio_write(conf->media.music.buffer, 0);
        //    (void)pa_stream_drain(conf->audio.session.output_stream, NULL, NULL);
        return 0;

    case MUSIC_STOP:
    default:
        //antfx_media_music_release_ctrl(&conf->media.music);
        //if(fd > 0) close(fd);
        return -1;
    }
}

int antfx_media_music_play(const char *song)
{
    int fd;
    int error = 0;
    antfx_conf_t *conf = antfx_get_config();
    conf->media.mode = M_NONE;
    LOG("Playing %s", song);
    if (conf->media.music.status != MUSIC_STOP)
    {
        antfx_media_music_stop();
    }
    fd = open(song, O_RDONLY);
    if ((conf->media.music.mh = mpg123_new(NULL, &errno)) == NULL)
    {
        ERROR("Basic setup goes wrong: %s", mpg123_plain_strerror(errno));
        return -1;
    }
    mpg123_handle *mh = (mpg123_handle *)conf->media.music.mh;
    if (fd == -1)
    {
        ERROR("Unable to open song %s: %s", song, strerror(errno));
        return -1;
    }
    error = mpg123_open_fd(mh, fd);
    if (error != MPG123_OK)
    {
        ERROR("MPG123: Unable to open file %s: %s", song, mpg123_strerror(mh));
        close(fd);
        return -1;
    }
    long rate;
    int channels;
    error = mpg123_getformat(mh, (long *)&rate, (int *)&channels, &conf->media.music.encoding);
    if (error != MPG123_OK)
    {
        ERROR("Unable to fetch media format: %s", mpg123_strerror(mh));
        antfx_media_music_release_ctrl(&conf->media.music);
        close(fd);
        return -1;
    }
    conf->media.music.total_frames = mpg123_framelength(mh);
    conf->media.mode = M_MUSIC_MODE;
    conf->media.music.status = MUSIC_PLAYING;
    if (antfx_audio_write_event_fd(fd, antfx_media_music_playback) == -1)
    {
        ERROR("Unable to set write event to mpg stream");
        antfx_media_music_release_ctrl(&conf->media.music);
        close(fd);
        conf->media.mode = M_NONE;
        conf->media.music.status = MUSIC_STOP;
        return -1;
    }
    LOG("Subscribed to audio write event");
    strncpy(conf->media.music.current_song, song, ANTFX_MAX_STR_BUFF_SZ);
    return 0;
}
void antfx_media_music_pause()
{
    SET_STATUS(MUSIC_PAUSE);
    antfx_audio_output_pause();
}
void antfx_media_music_resume()
{
    SET_STATUS(MUSIC_PLAYING);
    antfx_audio_output_resume();
}
void antfx_media_music_stop()
{
    SET_STATUS(MUSIC_STOP);
    while (conf->media.music.mh != NULL)
    {
        usleep(10000);
    }
    antfx_audio_flush();
}

void antfx_media_init()
{
    mpg123_init();
    antfx_conf_t *conf = antfx_get_config();
    conf->media.music.status = MUSIC_STOP;
    conf->media.music.mode = MUSIC_ONCE;
    conf->media.music.total_frames = 0;
    conf->media.music.current_frame = 0;
    conf->media.music.buffer = NULL;
    conf->media.music.buffer_len = 0;
    conf->media.music.songs = NULL;
    conf->media.music.total_songs = 0;
    conf->media.music.current_page = 0;
    memset(conf->media.music.current_song, 0, ANTFX_MAX_STR_BUFF_SZ);
    conf->media.mode = M_NONE;
    // start audio system in another thread
    pthread_t tid;
    if (pthread_create(&tid, NULL,(void *(*)(void *))antfx_media_audio_thread, NULL) != 0)
    {
        ERROR("pthread_create: cannot create audio thread: %s", strerror(errno));
    }
    else
    {
        if(pthread_detach(tid) != 0)
        {
            ERROR("Unable to detach thread");
        }
    }
    // antfx_audio_init();
    // load the music list
    antfx_audio_music_load_playlist();
}
void antfx_media_release()
{
    antfx_conf_t *conf = antfx_get_config();
    mpg123_exit();
    antfx_media_music_stop();
    antfx_audio_release();
    if (conf->media.music.songs)
    {
        bst_free(conf->media.music.songs, 1);
        conf->media.music.songs = NULL;
    }
}

int antfx_media_fm_start(float freq, int id)
{
    antfx_conf_t *conf = antfx_get_config();
    antfx_media_music_stop();
    conf->media.mode = M_NONE;
    antfx_hw_fm_set_freq(freq);
    if (antfx_audio_set_input(id, "FM-in", antfx_media_fm_cb) == -1)
    {
        antfx_hw_fm_mute();
        ERROR("Unable to connect radio input to output");
        return -1;
    }
    conf->media.mode = M_FM_MODE;
    return 0;
}
void antfx_media_fm_stop()
{
    antfx_conf_t *conf = antfx_get_config();
    if (conf->media.mode != M_FM_MODE)
        return;

    antfx_audio_remove_input();
    antfx_hw_fm_mute();
    antfx_audio_flush();
    conf->media.mode = M_NONE;
}