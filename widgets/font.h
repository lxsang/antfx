#ifndef AFX_FONT_H
#define AFX_FONT_H 

#define MAGIC_HEADER 0xAF8D

#include "../supports.h"

#define BIT0 (1 << 0)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)
#define BIT8 (1 << 8)


typedef struct {
    uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} afx_font_glyph_t;

typedef struct{
    uint8_t* data;
    uint16_t size;
    afx_font_glyph_t* glyphs;
    uint16_t gsize;
    char* name;
    uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;
    uint8_t loaded;
} afx_font_t;

extern afx_font_t SYS_FONT;

void _put_text(const char*,point_t, color_t, afx_font_t);

int load_font(const char* file, afx_font_t*);
void font_release(afx_font_t*);
#endif
