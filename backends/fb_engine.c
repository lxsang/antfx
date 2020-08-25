#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "../backend.h"

void display_init(engine_frame_t* frame, engine_config_t conf)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint8_t *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;
    if(conf.dev == NULL)
    {
        ERROR("No framebuffer device found in configuration");
        exit(1);
    }
    // Open the file for reading and writing
    frame->handle = open(conf.dev, O_RDWR);
    if ( frame->handle == -1) {
        ERROR("Error: cannot open framebuffer device: %s", strerror(errno));
        exit(1);
    }
    LOG("The framebuffer device was opened successfully.");

    // Get fixed screen information
    if (ioctl( frame->handle, FBIOGET_FSCREENINFO, &finfo) == -1) {
        ERROR("Error reading fixed screen information: %s", strerror(errno));
        exit(1);
    }

    // Get variable screen information
    if (ioctl( frame->handle, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        ERROR("Error reading variable information:%s", strerror(errno));
        exit(1);
    }

    LOG("%dx%d, %dbpp", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    frame->width = vinfo.xres;
    frame->height = vinfo.yres;
    frame->bbp = vinfo.bits_per_pixel;
    frame->xoffset = vinfo.xoffset;
    frame->yoffset = vinfo.yoffset;
    frame->line_length = finfo.line_length;
    // Figure out the size of the screen in bytes
    frame->size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    frame->buffer = (uint8_t *)mmap(0, frame->size, PROT_READ | PROT_WRITE, MAP_SHARED, frame->handle, 0);
    if ((int)fbp == -1) {
        ERROR("Error: failed to map framebuffer device to memory: %s", strerror(errno));
        exit(1);
    }
    // now open the touch device
    if(conf.tdev != NULL)
    {
        frame->handle = open(conf.dev, O_RDONLY);
        if(frame->handle == -1)
        {
            ERROR("Unable to open touch device, touch is disabled: %s", strerror(errno));
        }
    }
    

    LOG("The framebuffer device was mapped to memory successfully.");
    LOG("engine init successful");
}

void display_release(engine_frame_t* frame)
{
    if(frame->buffer)
        munmap(frame->buffer, frame->size);
    if(frame->handle != -1)
        close(frame->handle);
    LOG("release successful");
}

void get_pointer_input(antfx_event_t* event)
{
    event->type = AFX_EVT_NONE;
}
