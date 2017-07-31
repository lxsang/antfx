#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main()
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint8_t *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb1", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp xof %d yof %d, line length %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, vinfo.xoffset, vinfo.yoffset, finfo.line_length);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    x = 100; y = 100;       // Where we are going to put the pixel

    // Figure out where in memory to put the pixel
    for (y = 100; y < 300; y++)
        for (x = 100; x < 300; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {
                *(fbp + location) = 100;        // Some blue
                *(fbp + location + 1) = 15+(x-100)/2;     // A little green
                *(fbp + location + 2) = 200-(y-100)/5;    // A lot of red
                *(fbp + location + 3) = 0;      // No transparency
        //location += 4;
            } else  { //assume 16bpp
                int b = 10;
                int g = (x-100)/6;     // A little green
                int r = 31-(y-100)/16;    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }

        }
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
/*

{
       uint sampleRGB = 0xFF0000;
       uint R = ( sampleRGB >> 16 ) & 0xFF;
       uint G = ( sampleRGB >> 8 ) & 0xFF;
       uint B = sampleRGB & 0xFF;
       uint r = R * 31 / 255;
       uint g = G * 63 / 255;
       uint b = B * 31 / 255;
       ushort converted16bpp = (ushort)( r << ( 5 + 6 ) | ( g << 5 ) | b );
}
{
       ushort sample16bpp = 0xF800;
       uint r = (uint)( ( sample16bpp >> ( 5 + 6 ) ) & 31 );
       uint g = (uint)( ( sample16bpp >> 5 ) & 63 );
       uint b = (uint)( sample16bpp & 31 );
       uint R = r * 255 / 31;
       uint G = g * 255 / 63;
       uint B = b * 255 / 31;
       uint convertedRGB = ( R << 16 ) | ( G << 8 ) | B;
}*/