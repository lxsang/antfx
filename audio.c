#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "utils.h"
#include "audio.h"
#include "conf.h"
#include "log.h"

typedef enum
{
    RECORD,
    PLAYBACK
} antfx_audio_mode_t;
static pa_sample_spec sample_input_spec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2};
static pa_sample_spec sample_output_spec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2};

static int antfx_audio_pulse_connect(antfx_audio_t *, void *);
static void antfx_audio_pulse_release(antfx_audio_t *);
static void antfx_context_state_callback(pa_context *c, void *userdata);
static void antfx_audio_sink_info_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
static void antfx_audio_source_info_cb(pa_context *c, const pa_source_info *i, int eol, void *userdata);
static void antfx_audio_dev_dump(bst_node_t *, void **, int);
static int antfx_audio_create_stream(antfx_audio_session_t *, const char *, const char *, antfx_audio_mode_t, pa_sample_spec *, void *);
static void antfx_audio_disconnect_stream(pa_stream *);
static void antfx_audio_stream_write_callback(pa_stream *s, size_t length, void *userdata);
static void antfx_audio_stream_read_callback(pa_stream *s, size_t length, void *userdata);
static void antfx_audio_stream_state_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_suspended_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_underflow_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_overflow_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_started_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_moved_callback(pa_stream *s, void *userdata);
static void antfx_audio_stream_buffer_attr_callback(pa_stream *s, void *userdata);
static void antfx_audio_write_to_output(int, antfx_audio_session_t *);
static void stdin_callback(pa_mainloop_api *a, pa_io_event *e, int fd, pa_io_event_flags_t f, void *userdata);
//static void antfx_audio_time_event_callback(pa_mainloop_api *m, pa_time_event *e, const struct timeval *tv, void *userdata);

static void antfx_audio_write_to_output(int length, antfx_audio_session_t *session)
{
    size_t l;
    if (!session->output_stream || !session->buffer.mem || !session->buffer.len)
        return;

    l = length;
    if (l > session->buffer.len)
        l = session->buffer.len;
    if (pa_stream_write(session->output_stream, (uint8_t *)session->buffer.mem, l, NULL, 0, PA_SEEK_RELATIVE) < 0)
    {
        ERROR("pa_stream_write() failed: %s", pa_strerror(pa_context_errno(session->context)));
        antfx_audio_disconnect_stream(session->output_stream);
        session->output_stream = NULL;
        return;
    }
    memmove(session->buffer.mem, session->buffer.mem + l, session->buffer.len - l);
    session->buffer.len -= l;
}
/*
static void antfx_audio_time_event_callback(pa_mainloop_api *m, pa_time_event *e, const struct timeval *tv, void *userdata)
{
    antfx_audio_session_t *session = (antfx_audio_session_t *)userdata;
    int writable;

    struct timeval now;
    pthread_mutex_lock(&session->lock);
    if (session->output_stream != NULL && session->buffer.ready)
    {

        writable = pa_stream_writable_size(session->output_stream);
        if (writable)
        {
            antfx_audio_write_to_output(writable, session);
        }
    }
    pthread_mutex_unlock(&session->lock);
    gettimeofday(&now, NULL);
    pa_timeval_add(&now, 500);
    m->time_restart(e, &now);
}*/
/* This is called whenever new data may be written to the stream */
static void antfx_audio_stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
    assert(s);
    assert(length > 0);
    antfx_audio_session_t *session = (antfx_audio_session_t *)userdata;
    pthread_mutex_lock(&session->lock);
    int writable = pa_stream_writable_size(s);
    if (writable < length)
    {
       return;
    }
    if (session->buffer.ready)
    {
        antfx_audio_write_to_output(length, session);
    }
    pthread_mutex_unlock(&session->lock);
}

/* This is called whenever new data may is available */
static void antfx_audio_stream_read_callback(pa_stream *s, size_t length, void *userdata)
{
    const void *data;
    assert(s);
    assert(userdata);
    assert(length > 0);
    antfx_conf_t *conf = antfx_get_config();
    antfx_audio_session_t *session = &conf->audio.session;
    int (*callback)(void *, int) = (int (*)(void *, int))userdata;
    if (session->input_stream != s)
    {
        return;
    }
    while (pa_stream_readable_size(s) > 0)
    {
        if (pa_stream_peek(s, &data, &length) < 0)
        {
            ERROR("pa_stream_peek() failed: %s\n", pa_strerror(pa_context_errno(session->context)));
            antfx_audio_disconnect_stream(s);
            if (s == session->input_stream)
            {
                session->input_stream = NULL;
            }
            else if (s == session->output_stream)
            {
                session->output_stream = NULL;
            }
            return;
        }

        assert(data);
        assert(length > 0);
        if (callback && callback((void *)data, length) == 0)
        {
            pa_stream_drop(s);
        }
        else
        {
            break;
        }
    }
}

/* This routine is called whenever the stream state changes */
static void antfx_audio_stream_state_callback(pa_stream *s, void *userdata)
{
    assert(s);
    assert(userdata);
    antfx_audio_session_t *session = (antfx_audio_session_t *)userdata;
    const pa_buffer_attr *a;
    char cmt[PA_CHANNEL_MAP_SNPRINT_MAX], sst[PA_SAMPLE_SPEC_SNPRINT_MAX];
    switch (pa_stream_get_state(s))
    {
    case PA_STREAM_CREATING:
        break;
    case PA_STREAM_TERMINATED:
        LOG("Stream device terminated");
        break;

    case PA_STREAM_READY:

        LOG("Stream successfully created");

        if (!(a = pa_stream_get_buffer_attr(s)))
            ERROR("pa_stream_get_buffer_attr() failed: %s", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
        else
        {

            LOG("Buffer metrics: maxlength=%u, tlength=%u, prebuf=%u, minreq=%u fragsize=%u", a->maxlength, a->tlength, a->prebuf, a->minreq, a->fragsize);
        }

        LOG("Using sample spec '%s', channel map '%s'",
            pa_sample_spec_snprint(sst, sizeof(sst), pa_stream_get_sample_spec(s)),
            pa_channel_map_snprint(cmt, sizeof(cmt), pa_stream_get_channel_map(s)));

        LOG("Connected to device %s (%u, %ssuspended).",
            pa_stream_get_device_name(s),
            pa_stream_get_device_index(s),
            pa_stream_is_suspended(s) ? "" : "not ");
        if (!session->buffer.mem)
        {
            pthread_mutex_lock(&session->lock);
            session->buffer.mem = pa_xmalloc(MAX_AUDIO_INPUT_BUFFER);
            session->buffer.len = 0;
            session->buffer.ready = 0;
            pthread_mutex_unlock(&session->lock);
        }
        if (s == session->output_stream)
        {
            session->buffer.writable = 1;
        }
        break;

    case PA_STREAM_FAILED:
    default:
        antfx_audio_disconnect_stream(s);
        if (s == session->input_stream)
        {
            session->input_stream = NULL;
        }
        else if (s == session->output_stream)
        {
            session->output_stream = NULL;
        }
        ERROR("Stream %s error: %s", pa_stream_get_device_name(s), pa_strerror(pa_context_errno(pa_stream_get_context(s))));
    }
}

static void antfx_audio_stream_suspended_callback(pa_stream *s, void *userdata)
{
    assert(s);

    if (pa_stream_is_suspended(s))
        LOG("Stream %s device suspended", pa_stream_get_device_name(s));
    else
        LOG("Stream %s device resumed", pa_stream_get_device_name(s));
}

static void antfx_audio_stream_underflow_callback(pa_stream *s, void *userdata)
{
    assert(s);

    LOG("Stream %s underrun", pa_stream_get_device_name(s));
}

static void antfx_audio_stream_overflow_callback(pa_stream *s, void *userdata)
{
    assert(s);

    LOG("Stream %s overrun", pa_stream_get_device_name(s));
}

static void antfx_audio_stream_started_callback(pa_stream *s, void *userdata)
{
    assert(s);

    LOG("Stream %s started", pa_stream_get_device_name(s));
}

static void antfx_audio_stream_moved_callback(pa_stream *s, void *userdata)
{
    assert(s);

    LOG("Stream moved to device %s (%u, %ssuspended).", pa_stream_get_device_name(s), pa_stream_get_device_index(s), pa_stream_is_suspended(s) ? "" : "not ");
}

static void antfx_audio_stream_buffer_attr_callback(pa_stream *s, void *userdata)
{
    assert(s);
    LOG("Stream %s buffer attributes changed.", pa_stream_get_device_name(s));
}

static void antfx_audio_stream_event_callback(pa_stream *s, const char *name, pa_proplist *pl, void *userdata)
{
    char *t;

    assert(s);
    assert(name);
    assert(pl);
    t = pa_proplist_to_string_sep(pl, ", ");
    LOG("Stream: %s, Got event '%s', properties '%s'", pa_stream_get_device_name(s), name, t);
    pa_xfree(t);
}

static int antfx_audio_create_stream(antfx_audio_session_t *session, const char *name, const char *dev_name, antfx_audio_mode_t mode, pa_sample_spec *spec, void *userdata)
{
    pa_stream **stream = NULL;
    if (mode == PLAYBACK)
    {
        stream = &session->output_stream;
    }
    else
    {
        stream = &session->input_stream;
    }
    if (*stream)
    {
        antfx_audio_disconnect_stream(*stream);
    }
    if (!(*stream = pa_stream_new(session->context, name, spec, NULL)))
    {
        ERROR("pa_stream_new() failed: %s", pa_strerror(pa_context_errno(session->context)));
        return -1;
    }

    pa_stream_set_state_callback(*stream, antfx_audio_stream_state_callback, session);
    pa_stream_set_write_callback(*stream, antfx_audio_stream_write_callback, session);
    pa_stream_set_read_callback(*stream, antfx_audio_stream_read_callback, userdata);
    pa_stream_set_suspended_callback(*stream, antfx_audio_stream_suspended_callback, NULL);
    pa_stream_set_moved_callback(*stream, antfx_audio_stream_moved_callback, NULL);
    pa_stream_set_underflow_callback(*stream, antfx_audio_stream_underflow_callback, session);
    pa_stream_set_overflow_callback(*stream, antfx_audio_stream_overflow_callback, NULL);
    pa_stream_set_started_callback(*stream, antfx_audio_stream_started_callback, NULL);
    pa_stream_set_event_callback(*stream, antfx_audio_stream_event_callback, NULL);
    pa_stream_set_buffer_attr_callback(*stream, antfx_audio_stream_buffer_attr_callback, NULL);

    if (mode == PLAYBACK)
    {
        pa_cvolume cv;
        // volume_is_set ? pa_cvolume_set(&cv, sample_spec.channels, volume) :  NULL
        if (pa_stream_connect_playback(*stream, dev_name, NULL, PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE, NULL, NULL) < 0)
        {
            ERROR("pa_stream_connect_playback() failed: %s", pa_strerror(pa_context_errno(session->context)));
            return -1;
        }
    }
    else
    {
        if (pa_stream_connect_record(*stream, NULL, NULL, PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE) < 0)
        {
            ERROR("pa_stream_connect_record() failed: %s\n", pa_strerror(pa_context_errno(session->context)));
            return -1;
        }
    }
}

static void antfx_audio_source_info_cb(pa_context *c,
                                       const pa_source_info *i,
                                       int eol,
                                       void *userdata)
{
    if (!c || !i || eol > 0)
    {
        return;
    }
    antfx_audio_dev_list_t *dev_list = (antfx_audio_dev_list_t *)userdata;

    pa_sink_port_info **ports = NULL;
    pa_sink_port_info *port = NULL;
    pa_sink_port_info *active_port = NULL;
    const char *prop_key = NULL;
    void *prop_state = NULL;
    int j;
    if (regex_match("^.*\\.monitor$", i->name, 0, NULL))
    {
        return;
    }
    antfx_audio_dev_t *dev = malloc(sizeof(antfx_audio_dev_t));
    if (!dev)
    {
        ERROR("Unable to malloc audio device structure: %s", strerror(errno));
        return;
    }
    dev->map = i->channel_map;
    dev->card = i->card;
    dev->volume = i->volume;
    strncpy(dev->name, i->name, ANTFX_MAX_STR_BUFF_SZ);
    strncpy(dev->desc, i->description, ANTFX_MAX_STR_BUFF_SZ);
    dev_list->devices = bst_insert(dev_list->devices, dev_list->count, dev, 1);
    dev_list->count++;
    /*
    while ((prop_key = pa_proplist_iterate(i->proplist, &prop_state)))
    {
        const char *prop_value = pa_proplist_gets(i->proplist, prop_key);
        LOG("DEBUG %s %s", prop_key, prop_value);
    }
    LOG("DEBUG channel_map_can_balance %s, channel_map_count %d",
        pa_channel_map_can_balance(&i->channel_map) ? "TRUE" : "FALSE",
        i->channel_map.channels);
    for (j = 0; j < i->channel_map.channels; j++)
    {
        LOG("DEBUG channel_map %d", i->channel_map.map[j]);
    }
    ports = i->ports;
    for (j = 0; j < i->n_ports; j++)
    {
        port = ports[j];
        LOG("DEBUG port %s %s %s",
            port->name,
            port->description,
            port->available ? "TRUE" : "FALSE");
    }
    active_port = i->active_port;
    if (active_port)
    {
        LOG("DEBUG active_port %s %s %s",
            active_port->name,
            active_port->description,
            active_port->available ? "TRUE" : "FALSE");
    }
    for (j = 0; j < i->volume.channels; j++)
    {
        LOG("DEBUG volume_channel_value %d", i->volume.values[j]);
    }
    LOG("DEBUG sink %s %s base_volume %d muted %s",
        i->name,
        i->description,
        i->base_volume,
        i->mute ? "TRUE" : "FALSE");
        */
}

static void antfx_audio_sink_info_cb(pa_context *c,
                                     const pa_sink_info *i,
                                     int eol,
                                     void *userdata)
{
    if (!c || !i || eol > 0)
    {
        return;
    }
    antfx_audio_dev_list_t *dev_list = (antfx_audio_dev_list_t *)userdata;

    pa_sink_port_info **ports = NULL;
    pa_sink_port_info *port = NULL;
    pa_sink_port_info *active_port = NULL;
    const char *prop_key = NULL;
    void *prop_state = NULL;
    int j;

    antfx_audio_dev_t *dev = malloc(sizeof(antfx_audio_dev_t));
    if (!dev)
    {
        ERROR("Unable to malloc audio device structure: %s", strerror(errno));
        return;
    }
    dev->map = i->channel_map;
    dev->card = i->card;
    dev->volume = i->volume;
    strncpy(dev->name, i->name, ANTFX_MAX_STR_BUFF_SZ);
    strncpy(dev->desc, i->description, ANTFX_MAX_STR_BUFF_SZ);
    dev_list->devices = bst_insert(dev_list->devices, dev_list->count, dev, 1);
    dev_list->count++;
    /*
    while ((prop_key = pa_proplist_iterate(i->proplist, &prop_state)))
    {
        const char *prop_value = pa_proplist_gets(i->proplist, prop_key);
        LOG("DEBUG %s %s", prop_key, prop_value);
    }
    LOG("DEBUG channel_map_can_balance %s, channel_map_count %d",
        pa_channel_map_can_balance(&i->channel_map) ? "TRUE" : "FALSE",
        i->channel_map.channels);
    for (j = 0; j < i->channel_map.channels; j++)
    {
        LOG("DEBUG channel_map %d", i->channel_map.map[j]);
    }

    ports = i->ports;
    for (j = 0; j < i->n_ports; j++)
    {
        port = ports[j];
        LOG("DEBUG port %s %s %s",
            port->name,
            port->description,
            port->available ? "TRUE" : "FALSE");
    }

    active_port = i->active_port;
    if (active_port)
    {
        LOG("DEBUG active_port %s %s %s",
            active_port->name,
            active_port->description,
            active_port->available ? "TRUE" : "FALSE");
    }
    for (j = 0; j < i->volume.channels; j++)
    {
        LOG("DEBUG volume_channel_value %d", i->volume.values[j]);
    }
    LOG("DEBUG sink %s %s base_volume %d muted %s",
        i->name,
        i->description,
        i->base_volume,
        i->mute ? "TRUE" : "FALSE");*/
}

static void antfx_audio_disconnect_stream(pa_stream *stream)
{
    if (stream)
    {
        (void)pa_stream_drain(stream, NULL, NULL);
        pa_stream_disconnect(stream);
        pa_stream_unref(stream);
    }
}
static void antfx_audio_pulse_release(antfx_audio_t *au)
{
    LOG("Release pulseaudio connection");
    if (au->session.input_stream)
    {
        // TODO: wait for drain ?
        LOG("Disconnect input stream");
        antfx_audio_disconnect_stream(au->session.input_stream);
        au->session.input_stream = NULL;
    }
    if (au->session.output_stream)
    {
        LOG("Disconnect output stream");
        antfx_audio_disconnect_stream(au->session.output_stream);
        au->session.output_stream = NULL;
    }
    if (au->session.context)
    {
        LOG("Disconnect context");
        while (pa_context_drain(au->session.context, NULL, NULL) != NULL)
        {
            usleep(10000);
        }
        pa_context_disconnect(au->session.context);
        pa_context_unref(au->session.context);
        au->session.context = NULL;
    }
    if (au->session.mainloop)
    {
        if (au->session.mainloop_api)
        {
            if (au->session.io_event)
            {
                au->session.mainloop_api->io_free(au->session.io_event);
            }
            au->session.mainloop_api->quit(au->session.mainloop_api, 0);
        }
        pa_threaded_mainloop_free(au->session.mainloop);
        au->session.mainloop = NULL;
        au->session.mainloop_api = NULL;
    }
}
static void antfx_context_state_callback(pa_context *c, void *userdata)
{
    pa_operation *pa_op = NULL;
    antfx_conf_t *conf = antfx_get_config();
    antfx_audio_t *au = &conf->audio;
    void (*callback)(void) = (void (*)(void))userdata;
    assert(c);
    assert(au);
    switch (pa_context_get_state(c))
    {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
    {
        int r;
        pa_buffer_attr buffer_attr;

        assert(c);
        LOG("Connection to pulse audio is ready");
        pa_op = pa_context_get_sink_info_list(c, antfx_audio_sink_info_cb, &au->outputs);
        if (!pa_op)
        {
            ERROR("pa_context_get_sink_info_list() failed");
            return;
        }
        pa_operation_unref(pa_op);

        pa_op = pa_context_get_source_info_list(c, antfx_audio_source_info_cb, &au->inputs);
        if (!pa_op)
        {
            ERROR("pa_context_get_source_info_list() failed");
            return;
        }
        pa_operation_unref(pa_op);
        if (callback)
            callback();
        break;
    }

    case PA_CONTEXT_TERMINATED:
        LOG("Pulse audio context terminated");
        //antfx_audio_pulse_release(au);
        break;

    case PA_CONTEXT_FAILED:
    default:
        ERROR("Connection failure: %s", pa_strerror(pa_context_errno(c)));
        //antfx_audio_pulse_release(au);
    }

    return;
}

static int antfx_audio_pulse_connect(antfx_audio_t *au, void *userdata)
{
    au->session.context = pa_context_new(au->session.mainloop_api, "Antfx");
    if (!au->session.context)
    {
        ERROR("pa_context_new() failed\n");
        antfx_audio_pulse_release(au);
        return -1;
    }
    pa_context_set_state_callback(au->session.context, antfx_context_state_callback, userdata);
    if (pa_context_connect(au->session.context, NULL, PA_CONTEXT_NOFAIL, NULL) < 0)
    {
        if (pa_context_errno(au->session.context) == PA_ERR_INVALID)
        {
            ERROR("Connection to PulseAudio failed");
            antfx_audio_release();
            return -1;
        }
    }
    return 0;
}
static void antfx_audio_dev_dump(bst_node_t *node, void **argv, int argc)
{
    UNUSED(argv);
    UNUSED(argc);
    antfx_audio_dev_t *dev = (antfx_audio_dev_t *)node->data;
    if (!dev)
        return;
    LOG("Name: %s - %s (card %d), channels: %d",
        dev->desc,
        dev->name,
        dev->card,
        dev->map.channels);
}
static void antfx_audio_io_event_cb(pa_mainloop_api *a, pa_io_event *e, int fd, pa_io_event_flags_t f, void *userdata)
{
    antfx_conf_t *conf = antfx_get_config();
    assert(a == conf->audio.session.mainloop_api);
    assert(e);
    assert(userdata);
    unsigned char *buffer = NULL;
    if (conf->audio.session.io_event != e)
    {
        return;
    }
    int (*callback)(int, unsigned char **, int, antfx_audio_write_event_t) = (int (*)(int, unsigned char **, int, antfx_audio_write_event_t))userdata;
    int writable = antfx_audio_writable_size();
    int actual_len = 0;
    int ret;
    if (!antfx_audio_writable() || writable == 0)
    {
        ret = callback(fd, &buffer, 0, A_EVENT_PENDING);
        if (ret > 0)
            ret = 0;
    }
    else
    {
        ret = callback(fd, &buffer, writable, A_EVENT_OK);
    }
    if (ret > 0)
    {
        actual_len = ret;
        if (ret > writable)
        {
            actual_len = writable;
            LOG("Actual len is bigger than writable len. This should not happend");
        }
        (void)antfx_audio_write(buffer, actual_len);
    }
    if (ret == -1)
    {
        pthread_mutex_lock(&conf->audio.session.lock);
        conf->audio.session.mainloop_api->io_free(conf->audio.session.io_event);
        conf->audio.session.io_event = NULL;
        LOG("Quit the write event handle");
        if (callback)
            (void)callback(fd, &buffer, 0, A_EVENT_FAIL);
        pthread_mutex_unlock(&conf->audio.session.lock);
    }
}

int antfx_audio_init(void (*callback)(void))
{

    antfx_conf_t *conf = antfx_get_config();

    conf->audio.inputs.devices = NULL;
    conf->audio.inputs.count = 0;
    conf->audio.outputs.devices = NULL;
    conf->audio.outputs.count = 0;
    conf->audio.session.buffer.len = 0;
    conf->audio.session.buffer.mem = NULL;
    conf->audio.session.buffer.ready = 0;
    conf->audio.session.buffer.writable = 0;
    conf->audio.session.io_event = NULL;
    conf->audio.session.context = NULL;
    conf->audio.session.input_stream = NULL;
    conf->audio.session.output_stream = NULL;
    conf->audio.session.mainloop_api = NULL;
    conf->audio.session.mainloop = NULL;

    pthread_mutex_init(&conf->audio.session.lock, NULL);
    LOG("Init the audio system");
    conf->audio.session.mainloop = pa_threaded_mainloop_new();
    if (conf->audio.session.mainloop == NULL)
    {
        ERROR("pa_threaded_mainloop_new() failed");
        return -1;
    }
    conf->audio.session.mainloop_api = pa_threaded_mainloop_get_api(conf->audio.session.mainloop);
    if (conf->audio.session.mainloop_api == NULL)
    {
        ERROR("pa_threaded_mainloop_get_api() failed");
        pa_threaded_mainloop_free(conf->audio.session.mainloop);
        antfx_audio_pulse_release(&conf->audio);
        return -1;
    }
    /*
    struct timeval now;
    gettimeofday(&now, NULL);
    pa_timeval_add(&now, 500);

    if (!(conf->audio.session.time_event = conf->audio.session.mainloop_api->time_new(conf->audio.session.mainloop_api, &now, antfx_audio_time_event_callback, &conf->audio.session)))
    {
        ERROR("Unable to add time event");
        antfx_audio_pulse_release(&conf->audio);
        return -1;
    }*/
    if (pa_threaded_mainloop_start(conf->audio.session.mainloop) == -1)
    {
        ERROR("pa_threaded_mainloop_start() failed");
        antfx_audio_pulse_release(&conf->audio);
        return -1;
    }
    return antfx_audio_pulse_connect(&conf->audio, callback);
}

void antfx_audio_release()
{
    antfx_conf_t *conf = antfx_get_config();
    antfx_audio_pulse_release(&conf->audio);
    if (conf->audio.session.buffer.mem)
    {
        pa_xfree(conf->audio.session.buffer.mem);
    }
    if (conf->audio.inputs.devices)
    {
        bst_free(conf->audio.inputs.devices, 1);
    }
    if (conf->audio.outputs.devices)
    {
        bst_free(conf->audio.outputs.devices, 1);
    }
    pthread_mutex_destroy(&conf->audio.session.lock);
}
void antfx_audio_dump()
{
    antfx_conf_t *conf = antfx_get_config();
    LOG("Input devices:");
    bst_for_each(conf->audio.inputs.devices, antfx_audio_dev_dump, NULL, 0);
    LOG("Output devices:");
    bst_for_each(conf->audio.outputs.devices, antfx_audio_dev_dump, NULL, 0);
}

int antfx_audio_writable()
{
    antfx_conf_t *conf = antfx_get_config();
    return conf->audio.session.output_stream != NULL && conf->audio.session.buffer.writable;
}
int antfx_audio_writable_size()
{
    antfx_conf_t *conf = antfx_get_config();
    if (!conf->audio.session.buffer.writable)
        return 0;
    return MAX_AUDIO_INPUT_BUFFER - conf->audio.session.buffer.len;
}
void antfx_audio_flush()
{
    antfx_conf_t *conf = antfx_get_config();
    pthread_mutex_lock(&conf->audio.session.lock);
    if (conf->audio.session.output_stream)
    {
        (void)pa_stream_flush(conf->audio.session.output_stream, NULL, NULL);
    }
    conf->audio.session.buffer.ready = 0;
    conf->audio.session.buffer.len = 0;
    //(void)pa_stream_drain(conf->audio.session.output_stream, NULL, NULL);

    pthread_mutex_unlock(&conf->audio.session.lock);
}
int antfx_audio_write(void *data, int size)
{
    antfx_conf_t *conf = antfx_get_config();
    int writable;

    pthread_mutex_lock(&conf->audio.session.lock);
    if (conf->audio.session.output_stream != NULL && conf->audio.session.buffer.ready)
    {
        writable = pa_stream_writable_size(conf->audio.session.output_stream);
        if (writable > 0)
        {
            antfx_audio_write_to_output(writable, &conf->audio.session);
        }
    }
    pthread_mutex_unlock(&conf->audio.session.lock);

    writable = antfx_audio_writable_size();
    if (writable < size)
    {
        ERROR("Audio buffer overflow or output device is not available");
        return -1;
    }
    pthread_mutex_lock(&conf->audio.session.lock);
    memcpy((uint8_t *)conf->audio.session.buffer.mem + conf->audio.session.buffer.len, data, size);
    conf->audio.session.buffer.len += size;
    if (conf->audio.session.buffer.ready == 0 && conf->audio.session.buffer.len > MAX_AUDIO_INPUT_BUFFER / 2)
    {
        conf->audio.session.buffer.ready = 1;
    }
    pthread_mutex_unlock(&conf->audio.session.lock);
    return 0;
}
int antfx_audio_set_input(int id, const char *name, int (*callback)(void *, int))
{
    antfx_conf_t *conf = antfx_get_config();
    bst_node_t *node = bst_find(conf->audio.outputs.devices, id);
    antfx_audio_dev_t *dev;
    const char *dev_name = NULL;
    if (node == NULL || node->data == NULL)
    {
        ERROR("Can not find input device: %d. Fallback to default input device", id);
    }
    else
    {
        dev = (antfx_audio_dev_t *)node->data;
        dev_name = dev->name;
        //sample_output_spec.channels = dev->map.channels;
    }

    return antfx_audio_create_stream(&conf->audio.session, name, dev_name, RECORD, &sample_output_spec, callback);
}
int antfx_audio_set_output(int id, const char *name)
{
    antfx_conf_t *conf = antfx_get_config();
    bst_node_t *node = bst_find(conf->audio.outputs.devices, id);
    antfx_audio_dev_t *dev;
    const char *dev_name = NULL;
    conf->audio.session.buffer.writable = 0;
    antfx_audio_flush();
    if (node == NULL || node->data == NULL)
    {
        ERROR("Can not find output device: %d. Fallback to default output device", id);
    }
    else
    {
        dev = (antfx_audio_dev_t *)node->data;
        dev_name = dev->name;
        //sample_output_spec.channels = dev->map.channels;
    }

    return antfx_audio_create_stream(&conf->audio.session, name, dev_name, PLAYBACK, &sample_output_spec, NULL);
}

void antfx_audio_remove_input()
{
    antfx_conf_t *conf = antfx_get_config();
    antfx_audio_disconnect_stream(conf->audio.session.input_stream);
    conf->audio.session.input_stream = NULL;
}

int antfx_audio_write_event_fd(int fd, int (*callback)(int, unsigned char **, int, antfx_audio_write_event_t))
{
    antfx_conf_t *conf = antfx_get_config();
    pthread_mutex_lock(&conf->audio.session.lock);
    if (conf->audio.session.io_event)
    {
        conf->audio.session.mainloop_api->io_free(conf->audio.session.io_event);
        conf->audio.session.io_event = NULL;
    }
    conf->audio.session.io_event =
        conf->audio.session.mainloop_api->io_new(
            conf->audio.session.mainloop_api,
            fd,
            PA_IO_EVENT_INPUT,
            antfx_audio_io_event_cb, callback);
    pthread_mutex_unlock(&conf->audio.session.lock);
    if (!conf->audio.session.io_event)
    {
        ERROR("io_new() failed on %d", fd);
        conf->audio.session.io_event = NULL;
        return -1;
    }
}

void antfx_audio_output_pause()
{
    antfx_conf_t *conf = antfx_get_config();
    conf->audio.session.buffer.ready = 0;
}
void antfx_audio_output_resume()
{
    printf("resum the music\n");
    antfx_conf_t *conf = antfx_get_config();
    conf->audio.session.buffer.ready = 1;
    pthread_mutex_lock(&conf->audio.session.lock);
    if (conf->audio.session.output_stream != NULL && conf->audio.session.buffer.ready)
    {
        int writable = pa_stream_writable_size(conf->audio.session.output_stream);
        if (writable > 0)
        {
            antfx_audio_write_to_output(writable, &conf->audio.session);
        }
    }
    pthread_mutex_unlock(&conf->audio.session.lock);
}

int antfx_audio_output_is_paused()
{
    antfx_conf_t *conf = antfx_get_config();
    return pa_stream_is_corked(conf->audio.session.output_stream);
}
