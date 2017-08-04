#include "FreeMono12pt7b.h"
#include "FreeMono18pt7b.h"
#include "FreeMono24pt7b.h"
#include "FreeMono9pt7b.h"
#include "FreeMonoBold12pt7b.h"
#include "FreeMonoBold18pt7b.h"
#include "FreeMonoBold24pt7b.h"
#include "FreeMonoBold9pt7b.h"
#include "FreeMonoBoldOblique12pt7b.h"
#include "FreeMonoBoldOblique18pt7b.h"
#include "FreeMonoBoldOblique24pt7b.h"
#include "FreeMonoBoldOblique9pt7b.h"
#include "FreeMonoOblique12pt7b.h"
#include "FreeMonoOblique18pt7b.h"
#include "FreeMonoOblique24pt7b.h"
#include "FreeMonoOblique9pt7b.h"
#include "FreeSans12pt7b.h"
#include "FreeSans18pt7b.h"
#include "FreeSans24pt7b.h"
#include "FreeSans9pt7b.h"
#include "FreeSansBold12pt7b.h"
#include "FreeSansBold18pt7b.h"
#include "FreeSansBold24pt7b.h"
#include "FreeSansBold9pt7b.h"
#include "FreeSansBoldOblique12pt7b.h"
#include "FreeSansBoldOblique18pt7b.h"
#include "FreeSansBoldOblique24pt7b.h"
#include "FreeSansBoldOblique9pt7b.h"
#include "FreeSansOblique12pt7b.h"
#include "FreeSansOblique18pt7b.h"
#include "FreeSansOblique24pt7b.h"
#include "FreeSansOblique9pt7b.h"
#include "FreeSerif12pt7b.h"
#include "FreeSerif18pt7b.h"
#include "FreeSerif24pt7b.h"
#include "FreeSerif9pt7b.h"
#include "FreeSerifBold12pt7b.h"
#include "FreeSerifBold18pt7b.h"
#include "FreeSerifBold24pt7b.h"
#include "FreeSerifBold9pt7b.h"
#include "FreeSerifBoldItalic12pt7b.h"
#include "FreeSerifBoldItalic18pt7b.h"
#include "FreeSerifBoldItalic24pt7b.h"
#include "FreeSerifBoldItalic9pt7b.h"
#include "FreeSerifItalic12pt7b.h"
#include "FreeSerifItalic18pt7b.h"
#include "FreeSerifItalic24pt7b.h"
#include "FreeSerifItalic9pt7b.h"

#include <string.h>
typedef struct {
    char * name;
    GFXfont base;
} FontData;

void convert()
{
   int number = 48;
   FontData font_base[48] = {
        (FontData) {"FreeMono12pt7b.bf",FreeMono12pt7b},
        (FontData) {"FreeMono18pt7b.bf",FreeMono18pt7b},
        (FontData) {"FreeMono24pt7b.bf",FreeMono24pt7b},
        (FontData) {"FreeMono9pt7b.bf",FreeMono9pt7b},
        (FontData) {"FreeMonoBold12pt7b.bf",FreeMonoBold12pt7b},
        (FontData) {"FreeMonoBold18pt7b.bf",FreeMonoBold18pt7b},
        (FontData) {"FreeMonoBold24pt7b.bf",FreeMonoBold24pt7b},
        (FontData) {"FreeMonoBold9pt7b.bf",FreeMonoBold9pt7b},
        (FontData) {"FreeMonoBoldOblique12pt7b.bf",FreeMonoBoldOblique12pt7b},
        (FontData) {"FreeMonoBoldOblique18pt7b.bf",FreeMonoBoldOblique18pt7b},
        (FontData) {"FreeMonoBoldOblique24pt7b.bf",FreeMonoBoldOblique24pt7b},
        (FontData) {"FreeMonoBoldOblique9pt7b.bf",FreeMonoBoldOblique9pt7b},
        (FontData) {"FreeMonoOblique12pt7b.bf",FreeMonoOblique12pt7b},
        (FontData) {"FreeMonoOblique18pt7b.bf",FreeMonoOblique18pt7b},
        (FontData) {"FreeMonoOblique24pt7b.bf",FreeMonoOblique24pt7b},
        (FontData) {"FreeMonoOblique9pt7b.bf",FreeMonoOblique9pt7b},
        (FontData) {"FreeSans12pt7b.bf",FreeSans12pt7b},
        (FontData) {"FreeSans18pt7b.bf",FreeSans18pt7b},
        (FontData) {"FreeSans24pt7b.bf",FreeSans24pt7b},
        (FontData) {"FreeSans9pt7b.bf",FreeSans9pt7b},
        (FontData) {"FreeSansBold12pt7b.bf",FreeSansBold12pt7b},
        (FontData) {"FreeSansBold18pt7b.bf",FreeSansBold18pt7b},
        (FontData) {"FreeSansBold24pt7b.bf",FreeSansBold24pt7b},
        (FontData) {"FreeSansBold9pt7b.bf",FreeSansBold9pt7b},
        (FontData) {"FreeSansBoldOblique12pt7b.bf",FreeSansBoldOblique12pt7b},
        (FontData) {"FreeSansBoldOblique18pt7b.bf",FreeSansBoldOblique18pt7b},
        (FontData) {"FreeSansBoldOblique24pt7b.bf",FreeSansBoldOblique24pt7b},
        (FontData) {"FreeSansBoldOblique9pt7b.bf",FreeSansBoldOblique9pt7b},
        (FontData) {"FreeSansOblique12pt7b.bf",FreeSansOblique12pt7b},
        (FontData) {"FreeSansOblique18pt7b.bf",FreeSansOblique18pt7b},
        (FontData) {"FreeSansOblique24pt7b.bf",FreeSansOblique24pt7b},
        (FontData) {"FreeSansOblique9pt7b.bf",FreeSansOblique9pt7b},
        (FontData) {"FreeSerif12pt7b.bf",FreeSerif12pt7b},
        (FontData) {"FreeSerif18pt7b.bf",FreeSerif18pt7b},
        (FontData) {"FreeSerif24pt7b.bf",FreeSerif24pt7b},
        (FontData) {"FreeSerif9pt7b.bf",FreeSerif9pt7b},
        (FontData) {"FreeSerifBold12pt7b.bf",FreeSerifBold12pt7b},
        (FontData) {"FreeSerifBold18pt7b.bf",FreeSerifBold18pt7b},
        (FontData) {"FreeSerifBold24pt7b.bf",FreeSerifBold24pt7b},
        (FontData) {"FreeSerifBold9pt7b.bf",FreeSerifBold9pt7b},
        (FontData) {"FreeSerifBoldItalic12pt7b.bf",FreeSerifBoldItalic12pt7b},
        (FontData) {"FreeSerifBoldItalic18pt7b.bf",FreeSerifBoldItalic18pt7b},
        (FontData) {"FreeSerifBoldItalic24pt7b.bf",FreeSerifBoldItalic24pt7b},
        (FontData) {"FreeSerifBoldItalic9pt7b.bf",FreeSerifBoldItalic9pt7b},
        (FontData) {"FreeSerifItalic12pt7b.bf",FreeSerifItalic12pt7b},
        (FontData) {"FreeSerifItalic18pt7b.bf",FreeSerifItalic18pt7b},
        (FontData) {"FreeSerifItalic24pt7b.bf",FreeSerifItalic24pt7b},
        (FontData) {"FreeSerifItalic9pt7b.bf",FreeSerifItalic9pt7b}
    };
    int i = 0;
    int16_t v = MAGIC_HEADER;
    for(i=0;i <  number;i++)
    {
        FontData d = font_base[i];
        unsigned char buffer[8];
        
        FILE *ptr;
        ptr = fopen(d.name,"wb");
        // write header of 8 bytes
        memcpy(buffer,&v,2);
        buffer[2] = d.base.first;
        buffer[3] = d.base.last;
        buffer[4] = d.base.yAdvance;
        memcpy(buffer+5,&d.base.size,2);
        fwrite(buffer,sizeof(buffer),1,ptr);
        // write the glyph table
        fwrite(d.base.glyph, 95*sizeof(GFXglyph),1,ptr );
        // wirte bitmap data at the end
        fwrite(d.base.bitmap, d.base.size,1,ptr);
        fclose(ptr);
        printf("Generated %s \n",d.name);
    }
}

int main(int argc, char** argv)
{
    convert();
}