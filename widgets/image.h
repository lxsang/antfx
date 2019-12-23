#ifndef AFX_IMAGE_H
#define AFX_IMAGE_H
#include "../supports.h"

#define draw_bitmap(x) (_draw_bitmap(x,ORIGIN))

typedef struct 
{
    uint32_t hsize;             //specifies the number of bytes required by the struct
    int32_t width;                      //specifies width in pixels
    int32_t height;                     //species height in pixels
    uint16_t planes;          //specifies the number of color planes, must be 1
    uint16_t bbp;             //specifies the number of bit per pixel
    uint32_t compression;    //spcifies the type of compression
    uint32_t size;              //size of image in bytes
    int32_t pxpm;                       //number of pixels per meter in x axis
    int32_t pypm;                       //number of pixels per meter in y axis
    uint32_t ncolors;           //number of colors used by th ebitmap
    uint32_t n_important_colors;//number of colors that are important
}afx_bitmap_tag_t;

typedef struct
{
    afx_bitmap_tag_t header;
    uint8_t* data;
    uint8_t loaded;
} afx_bitmap_t;

int read_bitmap_file(const char*, afx_bitmap_t*);
void _draw_bitmap(afx_bitmap_t,point_t tr);
void release_bitmap(afx_bitmap_t*);
#endif