
#include <stdio.h>
#include <string.h>
#include <pulse/error.h>
#include <pulse/simple.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "conf.h"

#define BUFFLEN 1024

int main(int argc, char const *argv[])
{
    pa_simple *source, *sink;
    pa_sample_spec spec;
    int error;
    unsigned char buffer[BUFFLEN];
    spec.format = PA_SAMPLE_S16LE;
    spec.channels = 2;
    spec.rate = 44100;
    source =  pa_simple_new(
        NULL,
        "antfx",
        PA_STREAM_RECORD,
        "antfx_source",
        //"alsa_input.usb-GeneralPlus_USB_Audio_Device-00.mono-fallback",
        "FM-in",
        &spec,
        NULL,
        NULL,
        &error);
    if (source == NULL)
    {
        ERROR("pa_simple_new: %s\n", pa_strerror(error));
        return 1;
    }

    sink =  pa_simple_new(
        NULL,
        "antfx",
        PA_STREAM_PLAYBACK,
        "alsa_output.1.stereo-fallback",
        "FM-out",
        &spec,
        NULL,
        NULL,
        &error);

    if (sink == NULL)
    {
        ERROR("pa_simple_new: %s\n", pa_strerror(error));
        pa_simple_free(source);
        return 1;
    }
    int len;
    int total_bytes = 0;
    int running = 1;
    while (pa_simple_read(source, buffer,BUFFLEN,&error) == 0)
    {
        LOG("Playing data");
        total_bytes += len;
        if (pa_simple_write(sink, buffer, BUFFLEN, &error) != 0)
        {
            ERROR("pa_simple_write: %s\n", pa_strerror(error));
            break;
        }
    }
    LOG("total byte read:%d (%d)", total_bytes, len);
    if(len < 0)
    {
        ERROR("pa_simple_write: %s\n", pa_strerror(error));
    }

    if (pa_simple_drain(sink, &error) != 0)
    {
        ERROR("pa_simple_drain: %s\n", pa_strerror(error));
    }
    pa_simple_free(sink);
    pa_simple_free(source);
    return 0;
}
