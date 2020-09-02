
#include <stdio.h>
#include <string.h>
#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "conf.h"

#define SINK "alsa_output.1.stereo-fallback"
#define SOURCE "alsa_input.usb-GeneralPlus_USB_Audio_Device-00.mono-fallback"

#define BUFFLEN 1024

static pa_context *g_ctx;
static pa_mainloop_api *g_mainloop_api;
static pa_threaded_mainloop *g_thread_mainloop = NULL;
static int channels;

static void pa_sink_info_cb(pa_context *c,
                            const pa_sink_info *i,
                            int eol,
                            void *userdata);

static void pa_sink_event_cb(pa_context *c,
                             const pa_sink_info *info,
                             int eol,
                             void *user_data)
{
    if (!c || !info || eol > 0 || !user_data)
    {
        return;
    }

    printf("DEBUG %s\n", user_data);
    pa_sink_info_cb(c, info, eol, NULL);
}

static void pa_context_subscribe_cb(pa_context *c,
                                    pa_subscription_event_type_t t,
                                    uint32_t idx,
                                    void *user_data)
{
    if (!c)
    {
        printf("m_pa_context_subscribe_cb() invalid arguement\n");
        return;
    }

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
    {
    case PA_SUBSCRIPTION_EVENT_SINK:
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW)
        {
            pa_context_get_sink_info_by_index(c, idx, pa_sink_event_cb, "sink_new");
        }
        else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE)
        {
            pa_context_get_sink_info_by_index(c, idx, pa_sink_event_cb, "sink_changed");
        }
        break;
        /* TODO: it does not need to test so much kind of event signal
        case PA_SUBSCRIPTION_EVENT_SOURCE:                                      
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
                pa_context_get_source_info_by_index(c, idx, m_pa_source_new_cb, NULL);
            } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_context_get_source_info_by_index(c, idx, m_pa_source_changed_cb, NULL);
            }                                                                   
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:                                  
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
                pa_context_get_sink_input_info(c, idx, m_pa_sink_input_new_cb, NULL);
            } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_context_get_sink_input_info(c, idx, m_pa_sink_input_changed_cb, NULL);
            }                                                                   
            break;                                                              
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:                               
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
                pa_context_get_source_output_info(c, idx, m_pa_source_output_new_cb, NULL);
            } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_context_get_source_output_info(c, idx, m_pa_source_output_changed_cb, NULL);
            }                                                                   
            break;                                                              
        case PA_SUBSCRIPTION_EVENT_CLIENT:                                      
            break;                                                              
        case PA_SUBSCRIPTION_EVENT_SERVER:                                      
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
                pa_context_get_server_info(c, m_pa_server_new_cb, NULL);        
            } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_context_get_server_info(c, m_pa_server_changed_cb, NULL);    
            }                                                                   
            break;
        case PA_SUBSCRIPTION_EVENT_CARD:                                        
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
                pa_context_get_card_info_by_index(c, idx, m_pa_card_new_cb, NULL);
            } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_context_get_card_info_by_index(c, idx, m_pa_card_changed_cb, NULL);
            }                                                                   
            break;
        */
    }
}

static void pa_sink_info_cb(pa_context *c,
                            const pa_sink_info *i,
                            int eol,
                            void *userdata)
{
    if (!c || !i || eol > 0)
    {
        return;
    }

    pa_sink_port_info **ports = NULL;
    pa_sink_port_info *port = NULL;
    pa_sink_port_info *active_port = NULL;
    const char *prop_key = NULL;
    void *prop_state = NULL;
    int j;

    while ((prop_key = pa_proplist_iterate(i->proplist, &prop_state)))
    {
        printf("DEBUG %s %s\n", prop_key, pa_proplist_gets(i->proplist, prop_key));
    }

    channels = i->channel_map.channels;
    printf("DEBUG channel_map_can_balance %s, channel_map_count %d\n",
           pa_channel_map_can_balance(&i->channel_map) ? "TRUE" : "FALSE",
           i->channel_map.channels);
    for (j = 0; j < i->channel_map.channels; j++)
    {
        printf("DEBUG channel_map %d\n", i->channel_map.map[j]);
    }

    ports = i->ports;
    for (j = 0; j < i->n_ports; j++)
    {
        port = ports[j];
        printf("DEBUG port %s %s %s\n",
               port->name,
               port->description,
               port->available ? "TRUE" : "FALSE");
    }

    active_port = i->active_port;
    if (active_port)
    {
        printf("DEBUG active_port %s %s %s\n",
               active_port->name,
               active_port->description,
               active_port->available ? "TRUE" : "FALSE");
    }

    for (j = 0; j < i->volume.channels; j++)
    {
        printf("DEBUG volume_channel_value %d\n", i->volume.values[j]);
    }

    printf("DEBUG sink %s %s base_volume %d muted %s\n",
           i->name,
           i->description,
           i->base_volume,
           i->mute ? "TRUE" : "FALSE");
}

static void context_state_cb(pa_context *c, void *user_data)
{
    if (!c)
    {
        printf("m_context_state_cb() invalid argument\n");
        return;
    }

    pa_operation *pa_op = NULL;

    switch (pa_context_get_state(c))
    {
    case PA_CONTEXT_UNCONNECTED:
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
        pa_context_set_subscribe_callback(c, pa_context_subscribe_cb, NULL);

        pa_op = pa_context_subscribe(c, (pa_subscription_mask_t)(PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE | PA_SUBSCRIPTION_MASK_SINK_INPUT | PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT | PA_SUBSCRIPTION_MASK_CLIENT | PA_SUBSCRIPTION_MASK_SERVER | PA_SUBSCRIPTION_MASK_CARD), NULL, NULL);
        if (!pa_op)
        {
            printf("pa_context_subscribe() failed\n");
            return;
        }
        pa_operation_unref(pa_op);

        pa_op = pa_context_get_sink_info_list(c, pa_sink_info_cb, NULL);
        if (!pa_op)
        {
            printf("pa_context_get_sink_info_list() failed\n");
            return;
        }
        pa_operation_unref(pa_op);
        break;

    case PA_CONTEXT_FAILED:
        if (g_ctx)
        {
            pa_context_unref(g_ctx);
            g_ctx = NULL;
        }

        printf("Connection failed, quit\n");
        return;

    case PA_CONTEXT_TERMINATED:
    default:
        printf("pa_context terminated\n");
        return;
    }
}
static void connect_to_pulse()
{
    g_ctx = pa_context_new(g_mainloop_api, "PulseAudio loopback");
    if (!g_ctx)
    {
        printf("pa_context_new() failed\n");
        return;
    }

    pa_context_set_state_callback(g_ctx, context_state_cb, NULL);

    if (pa_context_connect(g_ctx, NULL, PA_CONTEXT_NOFAIL, NULL) < 0)
    {
        if (pa_context_errno(g_ctx) == PA_ERR_INVALID)
        {
            printf("Connection to PulseAudio failed\n");
            return;
        }
    }
}

int main(int argc, char const *argv[])
{
    pa_sample_spec spec;
    int error;
    unsigned char buffer[BUFFLEN];
    spec.format = PA_SAMPLE_S16LE;
    spec.channels = 2;
    spec.rate = 44100;
    g_thread_mainloop = pa_threaded_mainloop_new();
    if (!g_thread_mainloop)
    {
        printf("pa_threaded_mainloop_new() failed\n");
        return -1;
    }
    g_mainloop_api = pa_threaded_mainloop_get_api(g_thread_mainloop);
    if (!g_thread_mainloop)
    {
        printf("pa_threaded_mainloop_get_api() failed\n");
        return -1;
    }
    pa_threaded_mainloop_start(g_thread_mainloop);
    connect_to_pulse();
    while(1)
    {
        
    }
    return 0;
}
