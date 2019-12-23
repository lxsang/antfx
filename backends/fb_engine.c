#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "../backend.h"

void display_init(engine_frame_t* frame, engine_config_t conf)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint8_t *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

    // Open the file for reading and writing
    frame->handle = open(conf.dev, O_RDWR);
    if ( frame->handle == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl( frame->handle, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl( frame->handle, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
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
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
    printf("engine init successful\n");
}

void display_release(engine_frame_t* frame)
{
    if(frame->buffer)
        munmap(frame->buffer, frame->size);
    if(frame->handle != -1)
        close(frame->handle);
    printf("release successful\n");
}

antfx_event_t get_pointer_input(uint16_t* data)
{
    return AFX_EVT_NONE;
}